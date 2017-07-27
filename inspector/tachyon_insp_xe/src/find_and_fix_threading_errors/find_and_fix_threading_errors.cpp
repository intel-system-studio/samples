/*

!==============================================================
!
! SAMPLE SOURCE CODE - SUBJECT TO THE TERMS OF SAMPLE CODE LICENSE AGREEMENT,
! http://software.intel.com/en-us/articles/intel-sample-source-code-license-agreement/
!
! Copyright 2016 Intel Corporation
!
! THIS FILE IS PROVIDED "AS IS" WITH NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT
! NOT LIMITED TO ANY IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
! PURPOSE, NON-INFRINGEMENT OF INTELLECTUAL PROPERTY RIGHTS.
!
! =============================================================
*/
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

#include "tbb/task_scheduler_init.h"
#include "tbb/parallel_for.h"
#include "tbb/mutex.h"
//#include "tbb/blocked_range.h" /* old */
#include "tbb/blocked_range2d.h" /* new - fixing CQ DPD200202427 */

color col;

static color_t render_one_pixel (int x, int y, unsigned int *local_mbox, volatile unsigned int &serial,
                                 int startx, int stopx, int starty, int stopy)
{
    /* private vars moved inside loop */
    ray primary, sample;
    color avcol;
	//color col;
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
    
	col=trace(&primary);  //Threading Error: col is a global variable declared at line 80
	//2 ways to fix this threading error
		//	1) Make col a local variable
				// Comment out the declaration of col near line 80
				// Remove the comments on the declaration of col near line 88
		//	2) Add lock(s) around all the code which uses this variable (approximately lines 104 <-> 148)

    serial = primary.serial;

    /* perform antialiasing if enabled.. */
    if (scene.antialiasing > 0) {
        for (alias=0; alias < scene.antialiasing; alias++) {

            serial++; /* increment serial number */
            sample=primary;  /* copy the regular primary ray to start with */
            sample.serial = serial; 

			sample.d.x+=((rand() % 100) - 50) / jitterscale;
            sample.d.y+=((rand() % 100) - 50) / jitterscale;
            sample.d.z+=((rand() % 100) - 50) / jitterscale;

			avcol=trace(&sample);  

            serial = sample.serial; /* update our overall serial # */

            col.r += avcol.r;  //Threading Error: see comments near line 104
            col.g += avcol.g;  //Threading Error: see comments near line 104
            col.b += avcol.b;  //Threading Error: see comments near line 104
        }

        col.r /= (scene.antialiasing + 1.0);  //Threading Error: see comments near line 104
        col.g /= (scene.antialiasing + 1.0);  //Threading Error: see comments near line 104
        col.b /= (scene.antialiasing + 1.0);  //Threading Error: see comments near line 104
    }

    /* Handle overexposure and underexposure here... */
    R=(int) (col.r*255);  //Threading Error: see comments near line 104
    if (R > 255) R = 255;
    else if (R < 0) R = 0;

    G=(int) (col.g*255);  //Threading Error: see comments near line 104
    if (G > 255) G = 255;
    else if (G < 0) G = 0;

    B=(int) (col.b*255);  //Threading Error: see comments near line 104
    if (B > 255) B = 255;
    else if (B < 0) B = 0;

    return video->get_color(R, G, B);

}

/*class draw_task {
public:
    void operator() (const tbb::blocked_range<int> &r) const
    {
        // task-local storage
        unsigned int serial=1;
        unsigned int mboxsize = sizeof(unsigned int)*(max_objectid() + 20);
        unsigned int * local_mbox = (unsigned int *) malloc(mboxsize);  

		for (unsigned int i=0;i<(mboxsize/(sizeof(unsigned int)));i++) 
			local_mbox[i]=0;  

        for (int y = r.begin(); y != r.end(); ++y) { 
			{
				drawing_area * drawing = new drawing_area(startx, totaly-y, stopx-startx, 1); // * drawing = new 
				for (int x = startx ; x < stopx; x++) {
					color_t c = render_one_pixel (x, y, local_mbox, serial, startx, stopx, starty, stopy);
					drawing->put_pixel(c);
				}
				
				delete drawing;
			}
			if(!video->next_frame()) return;
		}
		free(local_mbox);
    }

    draw_task () {}
};*/ /* old */

class parallel_task {
public:
    void operator() (const tbb::blocked_range2d<int> &r) const
    {
        // task-local storage
        unsigned int serial = 1;
        unsigned int mboxsize = sizeof(unsigned int)*(max_objectid() + 20);
        unsigned int * local_mbox = (unsigned int *) alloca(mboxsize);
        memset(local_mbox,0,mboxsize);
        if(video->next_frame())
        {
            drawing_area drawing(r.cols().begin(), totaly-r.rows().end(), r.cols().end() - r.cols().begin(), r.rows().end()-r.rows().begin());
            for (int i = 1, y = r.rows().begin(); y != r.rows().end(); ++y, i++) {
                drawing.set_pos(0, drawing.size_y-i);
                for (int x = r.cols().begin(); x != r.cols().end(); x++) {
                    color_t c = render_one_pixel (x, y, local_mbox, serial, startx, stopx, starty, stopy);
                    drawing.put_pixel(c);
                }
            }
        }
    }

    parallel_task () {}
}; /* new - fixing CQ DPD200202427 */


void * thread_trace(thr_parms * parms)
{
    int n, nthreads = tbb::task_scheduler_init::automatic;
    char *nthreads_str = getenv ("TBB_NUM_THREADS");
    if (nthreads_str && (sscanf (nthreads_str, "%d", &n) > 0) && (n > 0)) nthreads = n;
    tbb::task_scheduler_init init (nthreads);

    // shared but read-only so could be private too
    all_parms = parms;
    scene = parms->scene;
    startx = parms->startx;
    stopx = parms->stopx;
    starty = parms->starty;
    stopy = parms->stopy;
    jitterscale = 40.0*(scene.hres + scene.vres);
    //totaly = parms->scene.vres-1; /* old */
    totaly = parms->scene.vres; /* new - fixing CQ DPD200202427 */

    int g, grain_size = 50;
    char *grain_str = getenv ("TBB_GRAINSIZE");
    if (grain_str && (sscanf (grain_str, "%d", &g) > 0) && (g > 0)) grain_size = g;
  
    // Uses the preview feature: auto_partitioner.
    // Note that no grainsize is provided to the blocked_range object.
    //tbb::parallel_for (tbb::blocked_range<int> (starty, stopy), draw_task (), tbb::auto_partitioner() ); /* old */
    tbb::parallel_for (tbb::blocked_range2d<int> (starty, stopy, grain_size, startx, stopx, grain_size), parallel_task (), tbb::simple_partitioner()); /* new - fixing CQ DPD200202427 */

    return(NULL);  
}
