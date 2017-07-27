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

#ifndef SEISMIC_VIDEO_H_
#define SEISMIC_VIDEO_H_

#include "../../common/gui/video.h"

class Universe;

class SeismicVideo : public video
{
#ifdef _WINDOWS
    #define MAX_LOADSTRING 100
    TCHAR szWindowClass[MAX_LOADSTRING];    // the main window class name
    WNDCLASSEX wcex;
#endif
    static const char * const titles[2];

    bool initIsParallel ;

    Universe &u_;
    int numberOfFrames_; // 0 means forever, positive means number of frames, negative is undefined
    int threadsHigh;
private:
    void on_mouse(int x, int y, int key);
    void on_process();

#ifdef _WINDOWS
public:
#endif
    void on_key(int key);

public:
    SeismicVideo(    Universe &u,int numberOfFrames, int threadsHigh, bool initIsParallel=true);
};
#endif /* SEISMIC_VIDEO_H_ */
