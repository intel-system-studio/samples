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

#include "tbb/task_scheduler_init.h"
#include "tbb/parallel_for.h"
#include "tbb/spin_mutex.h"
#include "tbb/blocked_range.h"

struct storage {
    unsigned int serial;
    unsigned int mboxsize; 
    unsigned int * local_mbox;
};

static tbb::spin_mutex MyMutex, MyMutex2;

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

class parallel_task {
public:
    void operator() (const tbb::blocked_range<int> &r) const
    {
	    // ADVISOR COMMENT Although m_storage was inside the "y" loop in tachyon_anotated.cpp, we
		// ADVISOR COMMENT only need 1 instance per task, and in this functor several iterations are executed
		// ADVISOR COMMENT by a single task.
		// task-local storage
        storage m_storage;
        m_storage.serial = 1;
        m_storage.mboxsize = sizeof(unsigned int)*(max_objectid() + 20);           
        m_storage.local_mbox = (unsigned int *) malloc(m_storage.mboxsize);
        memset(m_storage.local_mbox,0,m_storage.mboxsize);

        for (int y = r.begin(); y != r.end(); ++y) { 
            drawing_area drawing(startx, totaly-y, stopx-startx, 1);
            for (int x = startx; x < stopx; x++) {
                color_t c = render_one_pixel (x, y, m_storage.local_mbox, m_storage.serial, startx, stopx, starty, stopy);
                drawing.put_pixel(c);
            }
			{
				// ADVISOR COMMENT This lock protects global variable in "library" next_frame() routine (winvideo.h),
				// ADVISOR COMMENT and lock will be released via destructor even if there is a "return".
				tbb::spin_mutex::scoped_lock lockUntilScopeExit(MyMutex); 
                if(!video->next_frame())
                {
                    free(m_storage.local_mbox);
					return;
				}
			}
		}
		free(m_storage.local_mbox);
    }

    parallel_task () {}
};

static void parallel_thread (void)
{
    // Uses the auto_partitioner feature.
    // Note that no grainsize is provided to the blocked_range object.
    tbb::parallel_for (tbb::blocked_range<int> (starty, stopy), parallel_task (), tbb::auto_partitioner() );
}

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
    totaly = parms->scene.vres-1;
  
    parallel_thread ();

    return(NULL);  
}
