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

#ifndef IPP_BLUR_ROTATE_H_
#define IPP_BLUR_ROTATE_H_

#include "ippi.h"
#include "gui/video.h"

// The main processing class
class ipp_blur_rotate
{
    /* updated filter mask should be applied */
    bool bFilterUpdate;
    /* updated rotation coefficients (angle and shifts) should be applied */
    bool bRotateUpdate;
    /* redraw the window */
    bool bRedraw;

    /* source image size */
    IppiSize srcSize;
    /* destination image size (should include whole rotated image) */
    IppiSize dstSize;

    /* Pointer to the source image data */
    Ipp8u *pSrc;
    /* Pointer to the blurred image data */
    Ipp8u *pBlur;
    /* Pointer to the blurred and rotated image data */
    Ipp8u *pBlurRot;

    /* image steps */
    int srcStep, blurStep, blurRotStep;

    /* rotation angle */
    double angle;

    /* size of mask for image blurring */
    IppiSize maskSize;

public:
    /* Constructor */
    ipp_blur_rotate();

    /* Destructor */
   ~ipp_blur_rotate();

    /* load image data from bmp file */
    bool loadFileBMP( const char* bmpImageFile );

    /* Returns size of required rendering area */
    IppiSize windowSize() const { return dstSize; };

    /* Key processing */
    void onKey( int key );

    /* Run processing */
    void process(const drawing_memory &dm);
};


#endif /* IPP_BLUR_ROTATE_H_ */
