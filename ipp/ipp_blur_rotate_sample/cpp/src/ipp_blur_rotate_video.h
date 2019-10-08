/*
//==============================================================
//
// SAMPLE SOURCE CODE - SUBJECT TO THE TERMS OF SAMPLE CODE LICENSE AGREEMENT,
// http://software.intel.com/en-us/articles/intel-sample-source-code-license-agreement/
//
// Copyright 2005-2018 Intel Corporation
//
// THIS FILE IS PROVIDED "AS IS" WITH NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT
// NOT LIMITED TO ANY IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE, NON-INFRINGEMENT OF INTELLECTUAL PROPERTY RIGHTS.
//
// =============================================================
*/

#ifndef IPP_BLUR_ROTATE_VIDEO_H_
#define IPP_BLUR_ROTATE_VIDEO_H_

#include "ipp_blur_rotate.h"

extern video *v;

/*
// Time operations
*/
#ifdef _WIN32
void time_sleep(unsigned int msec)
{
    if (msec)
        Sleep(msec);
}

#else
#include <sched.h>

void time_sleep(unsigned int msec)
{
    if (msec)
        usleep(1000 * msec);
    else
        sched_yield();
}
#endif

class ipp_blur_rotate_video : public video
{
    ipp_blur_rotate *ibr;

private:

    void on_key( int key )
    {
        if ( ibr )
        {
            ibr->onKey( key );
        }
    }

    void on_process()
    {
        if ( ibr )
        {
            while (v->running)
            {
                ibr->process(get_drawing_memory());
                video::next_frame();
            #ifndef _WIN32
                time_sleep(10);
            #endif
            }
        }
    }

public:
    ipp_blur_rotate_video() :ibr(0)
    {
        title = "Intel(R) IPP: blur + rotate tutorial";
        v = this;
    }

    void set_object( ipp_blur_rotate &_ibr )
    {
        ibr = &_ibr;
    }
};

#endif /* IPP_BLUR_ROTATE_VIDEO_H_ */
