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

#include "ipps.h"
#include "ipp_blur_rotate.h"

#include "bmpreader.h"

#include <math.h>
#include <stdio.h>

char titleBuffer[256];

video *v;

#if (defined unix || defined UNIX)
#include <X11/keysym.h>
#define VK_LEFT     XK_Left
#define VK_UP       XK_Up
#define VK_RIGHT    XK_Right
#define VK_DOWN     XK_Down
#define VK_ESCAPE   XK_Escape
#elif defined __APPLE__
#define VK_LEFT     0xf702
#define VK_UP       0xf700
#define VK_RIGHT    0xf703
#define VK_DOWN     0xf701
#define VK_ESCAPE   0x1B
#endif

IppStatus warpAffine(Ipp8u* pSrc, IppiSize srcSize, int srcStep, Ipp8u* pDst, IppiSize dstSize, int dstStep, const double coeffs[2][3])
{
    /* Intel(R) IPP functions status */
    IppStatus status = ippStsNoErr;

    /* number of image channels */
    const Ipp32u numChannels = 3;

    /* border value to extend the source image */
    Ipp64f pBorderValue[numChannels];

    /* sizes for WarpAffine data structure, initialization buffer, work buffer */
    int specSize = 0, initSize = 0, bufSize = 0;

    /* pointer to work buffer */
    Ipp8u* pBuffer  = NULL;

    /* pointer to WarpAffine data structure */
    IppiWarpSpec* pSpec = NULL;

    /* set offset of the processing destination ROI */
    IppiPoint dstOffset = {0, 0};

    /* border type for affine transform */
    IppiBorderType borderType = ippBorderConst;

    /* direction of warp affine transform */
    IppiWarpDirection direction = ippWarpForward;

    /* set border value to extend the source image */
    for (int i = 0; i < numChannels; ++i) pBorderValue[i] = 255.0;

    /* computed buffer sizes for warp affine data structure and initialization buffer */
    status = ippiWarpAffineGetSize(srcSize, dstSize, ipp8u, coeffs, ippLinear, direction, borderType,
        &specSize, &initSize);

    /* allocate memory */
    pSpec = (IppiWarpSpec*)ippsMalloc_8u(specSize);

    /* initialize data for affine transform */
    if (status >= ippStsNoErr) status = ippiWarpAffineLinearInit(srcSize, dstSize, ipp8u, coeffs, direction, numChannels, borderType, pBorderValue, 0, pSpec);

    /* get work buffer size */
    if (status >= ippStsNoErr) status = ippiWarpGetBufferSize(pSpec, dstSize, &bufSize);

    /* allocate memory for work buffer */
    pBuffer = ippsMalloc_8u(bufSize);

    /* affine transform processing */
    if (status >= ippStsNoErr) status = ippiWarpAffineLinear_8u_C3R(pSrc, srcStep, pDst, dstStep, dstOffset, dstSize, pSpec, pBuffer);

    /* free allocated memory */
    ippsFree(pSpec);
    ippsFree(pBuffer);

    return status;
}

void ipp_blur_rotate::process(const drawing_memory &dm)
{
    IppStatus status = ippStsNoErr;
    /* number of image channels */
    int numChannels = 3;

    /* perform filtering */
    if (bFilterUpdate)
    {
        /* temporary work buffer */
        Ipp8u* pBuffer = NULL;
        /* buffer size for filtering */
        int bufSize = 0;

        /* Get work buffer size */
        status = ippiFilterBoxBorderGetBufferSize(srcSize,maskSize,ipp8u,3,&bufSize);

        /* allocate buffer memory */
        pBuffer = ippsMalloc_8u(bufSize);

        /* Image filtering */
        if (status >= ippStsNoErr) status = ippiFilterBoxBorder_8u_C3R(pSrc, srcStep, pBlur, blurStep, srcSize, maskSize, ippBorderRepl, NULL, pBuffer);

        /* filtration flag is dropped */
        bFilterUpdate = false;
        /* rotation operation should be applied after filtration */
        bRotateUpdate = true;

        /* free buffer memory */
        ippsFree(pBuffer);
    }

    /* perform rotation */
    if (bRotateUpdate)
    {
        IppiSize roiSize = {dstSize.width / 2, dstSize.height };
        Ipp8u* pDstRoi = pBlurRot;
        IppiRect srcRoi = {0};
        srcRoi.width  = srcSize.width;
        srcRoi.height = srcSize.height;

        /* affine transform coefficients */
        double coeffs[2][3]  = {0};

        /* affine transform bounds */
        double bound[2][2]   = {0};

        /* compute affine transform coefficients by angle and x- and y-shifts */
        if (status >= ippStsNoErr) status = ippiGetRotateTransform(angle, 0, 0, coeffs);

        /* get bounds of transformed image */
        if (status >= ippStsNoErr) status = ippiGetAffineBound(srcRoi, bound, coeffs);

        /* fit source image to dst */
        coeffs[0][2] = -bound[0][0] + (dstSize.width / 2.f  - (bound[1][0] - bound[0][0])) / 2.f;
        coeffs[1][2] = -bound[0][1] + (dstSize.height - (bound[1][1] - bound[0][1])) / 2.f;

        /* perform affine processing for the blurred image */
        if (status >= ippStsNoErr) status = warpAffine(pBlur, srcSize, blurStep, pDstRoi, roiSize, blurRotStep, coeffs);

        /* set destination ROI for the not blurred image */
        pDstRoi = pBlurRot + roiSize.width * numChannels;

        /* perform affine processing for the original image */
        if (status >= ippStsNoErr) status = warpAffine(pSrc, srcSize, srcStep, pDstRoi, roiSize, blurRotStep, coeffs);

        /* rotation flag is dropped */
        bRotateUpdate = false;

        /* needs to redraw the image */
        bRedraw       = true;
    }

    if (bRedraw)
    {
        drawing_area area( 0, 0, dstSize.width, dstSize.height, dm) ;

        /* pass information message to window's title */
    #if defined _WIN32
        sprintf_s(titleBuffer, sizeof(titleBuffer)/sizeof(titleBuffer[0]), "Intel(R) IPP: blur + rotate tutorial : rotation angle %.0f : box filter mask size {%d, %d}", angle - 360.0 * floor(angle / 360.0), maskSize.width, maskSize.height);
    #elif (defined unix || defined UNIX)
        sprintf(titleBuffer, "Intel(R) IPP: blur + rotate tutorial : rotation angle %.0f : box filter mask size {%d, %d}", angle - 360.0 * floor(angle / 360.0), maskSize.width, maskSize.height);
    #elif defined __APPLE__
        sprintf(titleBuffer, "Intel(R) IPP: blur + rotate tutorial : rotation angle %.0f : box filter mask size {%d, %d}", angle - 360.0 * floor(angle / 360.0), maskSize.width, maskSize.height);
    #endif
        v->title = titleBuffer;
        v->show_title();

        // fill the rendering area
        for (int y = 0; y < dstSize.height; ++y)
        {
            Ipp8u* dstt = pBlurRot + y * blurRotStep;
            area.set_pos( 0, y );
            for (int x = 0, j = 0; x < dstSize.width; ++x, j += 3)
            {
                area.put_pixel( v->get_color(dstt[j+2], dstt[j+1], dstt[j]) );
            }
        }

        bRedraw = false;
    }
}

/* Key processing */
void ipp_blur_rotate::onKey( int key )
{
    if (pSrc == NULL || pBlur ==NULL || pBlurRot == NULL) return;

    /* up or down arrow key is pressed */
    if (key == VK_UP || key == VK_DOWN)
    {
        /* max size of mask for image blurring */
        const IppiSize maxMaskSize = {31,31};

        /* increase or decrease mask size on 2 depending on the key */
        maskSize.width  = (key == VK_DOWN) ? maskSize.width  - 2 : maskSize.width  + 2;
        maskSize.height = (key == VK_DOWN) ? maskSize.height - 2 : maskSize.height + 2;

        /* check that both mask width and mask height are positive */
        if (maskSize.width  < 1) maskSize.width  = 1;
        if (maskSize.height < 1) maskSize.height = 1;

        /* check that both mask width and mask height are at more the maximum mask size */
        if (maskSize.width  > maxMaskSize.width)  maskSize.width  = maxMaskSize.width;
        if (maskSize.height > maxMaskSize.height) maskSize.height = maxMaskSize.height;

        /* filtration operation should be applied */
        bFilterUpdate = true;
    }

     /* left or right arrow key is pressed */
    if(key == VK_RIGHT || key == VK_LEFT)
    {
        /* increase or decrease angle on 2 depending on the key */
        angle = (key == VK_LEFT) ? angle + 2 : angle - 2;
        /* rotation operation should be applied after filtration */
        bRotateUpdate = true;
    }

    if (key == VK_ESCAPE) v->running = false;
}

bool ipp_blur_rotate::loadFileBMP( const char* bmpImageFile )
{
    /* check the pointer */
    if (bmpImageFile == NULL) return false;

    /* number of image channels */
    int numChannels = 0;

    /* Read data from a file (only bmp format is supported) */
    Status status = ReadFile(bmpImageFile, &pSrc, srcSize, srcStep, numChannels);

    if (numChannels != 3)
    {
        status = STS_ERR_UNSUPPORTED;
    }

    if (status == STS_OK)
    {
        /* set blurred image step */
        blurStep = srcStep;

        /* set rotated image size to keep whole rotated image */
        dstSize.width  = static_cast<int>(sqrt((float)srcSize.width  * srcSize.width + srcSize.height  * srcSize.height) + 0.5f) * 2;
        dstSize.height = static_cast<int>(sqrt((float)srcSize.width  * srcSize.width + srcSize.height  * srcSize.height) + 0.5f);

        /* set image step for blurred and rotated image */
        blurRotStep = dstSize.width * numChannels;

        /* Memory allocation for the intermediate images */
        pBlur = ippsMalloc_8u(srcStep * srcSize.height);

        /* Memory allocation for the intermediate images */
        pBlurRot = ippsMalloc_8u(dstSize.width * numChannels * dstSize.height);

        maskSize.width  = 3;
        maskSize.height = 3;

        bFilterUpdate = bRotateUpdate = bRedraw = true;

        return true;
    }

    return false;
}

ipp_blur_rotate::ipp_blur_rotate()
    : angle(0.0), pSrc(NULL), pBlur(NULL), pBlurRot(NULL),
      bFilterUpdate(false), bRotateUpdate(false), bRedraw(false),
      srcStep(0), blurStep(0), blurRotStep(0)
{
    dstSize.width  = srcSize.width  = maskSize.width = 0;
    dstSize.height = srcSize.height = maskSize.height = 0;
}

ipp_blur_rotate::~ipp_blur_rotate()
{
    ippsFree(pSrc);
    ippsFree(pBlur);
    ippsFree(pBlurRot);
}
