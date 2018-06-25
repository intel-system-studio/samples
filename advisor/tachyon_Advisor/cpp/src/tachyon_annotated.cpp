//=======================================================================
//
// SAMPLE SOURCE CODE - SUBJECT TO THE TERMS OF END-USER LICENSE AGREEMENT FOR
// INTEL(R) ADVISOR XE.
//
// Copyright (C) 2014 Intel Corporation. All rights reserved
//
// THIS FILE IS PROVIDED "AS IS" WITH NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT
// NOT LIMITED TO ANY IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE, NON-INFRINGEMENT OF INTELLECTUAL PROPERTY RIGHTS.
//
// ========================================================================


/*
    The original source for this example is
    Copyright (c) 1994-2008 John E. Stone
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:
    1. Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.
    3. The name of the author may not be used to endorse or promote products
       derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
    OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
    OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
    HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
    LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
    OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
    SUCH DAMAGE.
*/

#include "machine.h"
#include "types.h"
#include "macros.h"
#include "vector.h"
#include "tgafile.h"
#include "trace.h"
#include "light.h"
#include "shade.h"
#include "camera.h"
#include "util.h"
#include "intersect.h"
#include "global.h"
#include "ui.h"
#include "video.h"

//ADVISOR COMMENT: Note that the DEFAULT_MODELFILE=820spheres.dat in the tachyon.common project preprocessor 
//ADVISOR COMMENT: definitions for the debug build to decrease input set and runtime when Modeling Correctness

#include <advisor-annotate.h>

struct storage {
    unsigned int serial;
    unsigned int mboxsize; 
    unsigned int * local_mbox;
};

//ADVISOR COMMENT: The declaration of m_storage should be moved from here to inside the task to privatize it
storage m_storage;

// shared but read-only so could be private too
static thr_parms *all_parms;
static scenedef scene;
static int startx;
static int stopx;
static int starty;
static int stopy;
static flt jitterscale;
static int totaly;

static color_t render_one_pixel (int x, int y, unsigned int *local_mbox, unsigned int &serial,
                                 int startx, int stopx, int starty, int stopy)
{
    /* private vars moved inside loop */
    ray primary, sample;
    color col, avcol;
    int R,G,B;
    intersectstruct local_intersections;    
    int alias;
    /* end private */

    primary=camray(&scene, x, y);
    primary.intstruct = &local_intersections;
    primary.flags = RT_RAY_REGULAR;

    serial++;
    primary.serial = serial;  
    primary.mbox = local_mbox;
    primary.maxdist = FHUGE;
    primary.scene = &scene;
    col=trace(&primary);  

    serial = primary.serial;

    /* perform antialiasing if enabled.. */
    if (scene.antialiasing > 0) {
        for (alias=0; alias < scene.antialiasing; alias++) {

            serial++; /* increment serial number */
            sample=primary;  /* copy the regular primary ray to start with */
            sample.serial = serial; 

            {
                sample.d.x+=((std::rand() % 100) - 50) / jitterscale;
                sample.d.y+=((std::rand() % 100) - 50) / jitterscale;
                sample.d.z+=((std::rand() % 100) - 50) / jitterscale;
            }

            avcol=trace(&sample);  

            serial = sample.serial; /* update our overall serial # */

            col.r += avcol.r;
            col.g += avcol.g;
            col.b += avcol.b;
        }

        col.r /= (scene.antialiasing + 1.0);
        col.g /= (scene.antialiasing + 1.0);
        col.b /= (scene.antialiasing + 1.0);
    }

    /* Handle overexposure and underexposure here... */
    R=(int) (col.r*255);
    if (R > 255) R = 255;
    else if (R < 0) R = 0;

    G=(int) (col.g*255);
    if (G > 255) G = 255;
    else if (G < 0) G = 0;

    B=(int) (col.b*255);
    if (B > 255) B = 255;
    else if (B < 0) B = 0;

    return video->get_color(R, G, B);

}

static void parallel_thread (void)
{
	
	ANNOTATE_SITE_BEGIN(allRows);
	for (int y = starty; y < stopy; y++) 
	{ 
		ANNOTATE_TASK_BEGIN(eachRow);
		//ADVISOR COMMENT: The storage object m_storage is moved inside the task to privatize it and eliiminate the incidental sharing.
		//ADVISOR COMMENT: Uncomment the declaration of m_storage below
		//ADVISOR COMMENT: Don't forget to remove its declaration from the global scope at the top of this file.
		//storage m_storage;
		m_storage.serial = 1;
		m_storage.mboxsize = sizeof(unsigned int)*(max_objectid() + 20);           
		m_storage.local_mbox = (unsigned int *) malloc(m_storage.mboxsize);
		memset(m_storage.local_mbox,0,m_storage.mboxsize);

		drawing_area drawing(startx, totaly-y, stopx-startx, 1);
		for (int x = startx; x < stopx; x++) {
			color_t c = render_one_pixel (x, y, m_storage.local_mbox, m_storage.serial, startx, stopx, starty, stopy);
			drawing.put_pixel(c);
		} 

	    //ADVISOR COMMENT: The call to next_frame() causes a Data communication issue (race condition) when the global g_updates is incremented.
		//ADVISOR COMMENT: You can use locking in winvideo.h:next_frame() around the increment operation, or you can treat winvideo.h
		//ADVISOR COMMENT: like a library file that cannot be changed, and instead put the locking around the next_frame() call.
		//ADVISOR COMMENT: Uncomment the following three lock annotations to protect the access to the g_updates variable
		//ADVISOR COMMENT: (Why does RELEASE appear twice?)
		//ANNOTATE_LOCK_ACQUIRE(0);
		if(!video->next_frame())
		{
			//ANNOTATE_LOCK_RELEASE(0);
			free(m_storage.local_mbox);
			ANNOTATE_TASK_END(eachRow);
			ANNOTATE_SITE_END(allRows);
			return;
		}
		//ANNOTATE_LOCK_RELEASE(0);

		free(m_storage.local_mbox);       
		ANNOTATE_TASK_END(eachRow);
	}
	ANNOTATE_SITE_END(allRows);

}

void * thread_trace(thr_parms * parms)
{
    // shared but read-only so could be private too
    all_parms = parms;
    scene = parms->scene;
    startx = parms->startx;
    stopx = parms->stopx;
    starty = parms->starty;
    stopy = parms->stopy;
    jitterscale = 40.0*(scene.hres + scene.vres);
    totaly = parms->scene.vres-1;

    parallel_thread ();

    return(NULL);  
}
