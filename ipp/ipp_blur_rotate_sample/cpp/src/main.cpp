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

#define VIDEO_WINMAIN_ARGS

#include <stdio.h>
#include <iostream>

#include "ipp_blur_rotate.h"
#include "ipp_blur_rotate_video.h"

const char* filename = "./data/Image.bmp";

int main(int argc, char *argv[])
{
    try
    {
        ipp_blur_rotate_video video;
        ipp_blur_rotate iBlurRotate;

        if (iBlurRotate.loadFileBMP(filename))
        {
            IppiSize winSize = iBlurRotate.windowSize();
            // video layer init
            if (video.init_window(winSize.width, winSize.height) )
            {
                video.calc_fps = false;
                video.threaded = 1;
                // initialize ////////////
                video.set_object( iBlurRotate );
                // main loop
                video.main_loop();
            }
            else
            {
                std::cerr << "Cannot initialize video layer\n";
            }
        }
        else
        {
            std::cerr << "Cannot load the file: " << filename << "\n";
        }

        video.terminate();
    }
    catch ( std::exception& e )
    {
        std::cerr << "error occurred. error text is :\"" << e.what() << "\"\n";

        return 1;
    }

    return 0;
}
