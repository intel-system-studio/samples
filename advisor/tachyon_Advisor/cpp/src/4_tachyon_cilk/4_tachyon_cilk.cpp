//=======================================================================

// SAMPLE SOURCE CODE - SUBJECT TO THE TERMS OF END-USER LICENSE AGREEMENT FOR
// Intel® Advisor 2017.

// /* Copyright (C) 2010-2017 Intel Corporation. All Rights Reserved.
 
 // The source code, information and material ("Material") 
 // contained herein is owned by Intel Corporation or its 
 // suppliers or licensors, and title to such Material remains 
 // with Intel Corporation or its suppliers or licensors.
 // The Material contains proprietary information of Intel or 
 // its suppliers and licensors. The Material is protected by 
 // worldwide copyright laws and treaty provisions.
 // No part of the Material may be used, copied, reproduced, 
 // modified, published, uploaded, posted, transmitted, distributed 
 // or disclosed in any way without Intel's prior express written 
 // permission. No license under any patent, copyright or other
 // intellectual property rights in the Material is granted to or 
 // conferred upon you, either expressly, by implication, inducement, 
 // estoppel or otherwise. Any license under such intellectual 
 // property rights must be express and approved by Intel in writing.
 // Third Party trademarks are the property of their respective owners.
 // Unless otherwise agreed by Intel in writing, you may not remove 
 // or alter this notice or any other notice embedded in Materials 
 // by Intel or Intel's suppliers or licensors in any way.
 
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

// shared but read-only so could be private too
static thr_parms *all_parms;
static scenedef scene;
static int startx;
static int stopx;
static int starty;
static int stopy;
static flt jitterscale;
static int totaly;

#include <cilk/cilk.h>
#include "tbb/spin_mutex.h"

struct storage {
    unsigned int serial;
    unsigned int mboxsize; 
    unsigned int * local_mbox;
};

static tbb::spin_mutex MyMutex;

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
                //tbb::spin_mutex::scoped_lock lock (MyMutex);
                sample.d.x+=((rand() % 100) - 50) / jitterscale;
                sample.d.y+=((rand() % 100) - 50) / jitterscale;
                sample.d.z+=((rand() % 100) - 50) / jitterscale;
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
	// Cilk does not permit a return; from within a cilk_for, so instead we will
	// broadcast work_stopage via continue_work, which is global to all of the
	// cilk tasks.
	volatile bool continue_work = true;  

 	//ANNOTATE_SITE_BEGIN(allRows);
	cilk_for (int y = starty; y < stopy; y++) 
	{ 
		//ANNOTATE_TASK_BEGIN(eachRow);
		if (continue_work) {
		storage m_storage;
		m_storage.serial = 1;
		m_storage.mboxsize = sizeof(unsigned int)*(max_objectid() + 20);           
		m_storage.local_mbox = (unsigned int *) malloc(m_storage.mboxsize);
		memset(m_storage.local_mbox,0,m_storage.mboxsize);

		drawing_area drawing(startx, totaly-y, stopx-startx, 1);
		for (int x = startx; x < stopx; x++) {
			color_t c = render_one_pixel (x, y, m_storage.local_mbox, m_storage.serial, startx, stopx, starty, stopy);
			drawing.put_pixel(c);
		} 
		{
	    //ADVISOR COMMENT: The call to next_frame() causes a Data communication issue (race condition) when the global g_updates is incremented.
		//ADVISOR COMMENT: You can use locking in winvideo.h:next_frame() around the increment operation, or you can treat winvideo.h
		//ADVISOR COMMENT: like a library file that cannot be changed, and instead put the locking around the next_frame() call.
		tbb::spin_mutex::scoped_lock lockUntilScopeExit(MyMutex); 
		if(!video->next_frame())
		{
			//return;
			continue_work = false;  // broadcast "stop doing work" instead of the return
		}
		}
		free(m_storage.local_mbox);  
		}
		//ANNOTATE_TASK_END(eachRow);
	}
	//ANNOTATE_SITE_END(allRows);

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
