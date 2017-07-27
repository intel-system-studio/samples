/*
    ==============================================================

    SAMPLE SOURCE CODE - SUBJECT TO THE TERMS OF SAMPLE CODE LICENSE AGREEMENT,
    http://software.intel.com/en-us/articles/intel-sample-source-code-license-agreement/

    Copyright 2016 Intel Corporation

    THIS FILE IS PROVIDED "AS IS" WITH NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT
    NOT LIMITED TO ANY IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
    PURPOSE, NON-INFRINGEMENT OF INTELLECTUAL PROPERTY RIGHTS.

    =============================================================
*/

#ifndef FRACTAL_VIDEO_H_
#define FRACTAL_VIDEO_H_

#include "../../common/gui/video.h"
#include "fractal.h"

extern video *v;
extern bool single;

class fractal_video : public video
{
    fractal_group *fg;

private:
    void on_mouse( int x, int y, int key ) {
        if( key == 1 ) {
            if ( fg ) {
                fg->set_num_frames_at_least(20);
                fg->mouse_click( x, y );
            }
        }
    }

    void on_key( int key ) {
        switch ( key&0xff ) {
            case esc_key:
                running = false; break;
            case ' ': // space
                if( fg ) fg->switch_priorities(); break;

            case 'q':
                if( fg ) fg->active_fractal_zoom_in(); break;
            case 'e':
                if( fg ) fg->active_fractal_zoom_out(); break;

            case 'r':
                if( fg ) fg->active_fractal_quality_inc(); break;
            case 'f':
                if( fg ) fg->active_fractal_quality_dec(); break;

            case 'w':
                if( fg ) fg->active_fractal_move_up(); break;
            case 'a':
                if( fg ) fg->active_fractal_move_left(); break;
            case 's':
                if( fg ) fg->active_fractal_move_down(); break;
            case 'd':
                if( fg ) fg->active_fractal_move_right(); break;
        }
        if( fg ) fg->set_num_frames_at_least(20);
    }

    void on_process() {
        if ( fg ) {
            fg->run( !single );
        }
    }

public:
    fractal_video() :fg(0) {
        title = "Dynamic Priorities in TBB: Fractal Example";
        v = this;
    }

    void set_fractal_group( fractal_group &_fg ) {
        fg = &_fg;
    }
};

#endif /* FRACTAL_VIDEO_H_ */
