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

#include <ipps.h>
#include <math.h>
#include <stdio.h>
#include <iostream>

#include "bmpreader.h"

#if defined unix || defined UNIX
#include <string.h>
#endif

struct CodeStringTable
{
    int         iCode;
    const char *pString;
};

static const CodeStringTable StringOfBaseStatus[] = {
    { STS_OK,                   "Success" },
    { STS_ERR_FAILED,           "General failure" },
    { STS_ERR_NOT_INITIALIZED,  "Object is not initialized" },
    { STS_ERR_NOT_ENOUGH_DATA,  "Not enough input data" },
    { STS_ERR_NULL_PTR,         "Unexpected NULL pointer" },
    { STS_ERR_INIT,             "Failed to initialize object" },
    { STS_ERR_END_OF_STREAM,    "End of stream" },
    { STS_ERR_ALLOC,            "Failed to allocate memory" },
    { STS_ERR_UNSUPPORTED,      "Unsupported parameters/mode" },
    { STS_ERR_INVALID_PARAMS,   "Invalid parameters" },
    { STS_ERR_FILE_OPEN,        "Failed to open file" },
    { STS_ERR_FORMAT,           "Invalid format" },
};
static const char *sStatusEmpty = "<no status string>";

#define PRINT_FUNC_MESSAGE(STATUS, NAME, MESSAGE) printf("\nError %d in %s: %s\n", (int)STATUS, NAME, MESSAGE); fflush(0);
#define PRINT_MESSAGE(MESSAGE) printf("\nError: %s\n", MESSAGE); fflush(0);

#define CHECK_STATUS_PRINT_AC(STATUS, NAME, MESSAGE, ACTION) \
    if(STATUS != 0) { PRINT_FUNC_MESSAGE(STATUS, NAME, MESSAGE); if(STATUS < 0) {ACTION;} }

#define CHECK_STATUS_PRINT(STATUS, NAME, MESSAGE) \
    CHECK_STATUS_PRINT_AC(STATUS, NAME, MESSAGE,)

/* Utility functions */
unsigned int alignSize(unsigned int value, unsigned int alignment)
{
    return (unsigned int)((value + (alignment - 1)) & ~(alignment - 1));
}

const char* GetBaseStatusString(Status status)
{
    int iTableSize = sizeof(StringOfBaseStatus)/sizeof(StringOfBaseStatus[0]);

    for (int i = 0; i < iTableSize; i++)
    {
        if (StringOfBaseStatus[i].iCode == status)
            return StringOfBaseStatus[i].pString;
    }
    return sStatusEmpty;
}

/*
// Files operations
*/
#if defined _WIN32
#include <stdio.h>
#if !defined(__MIC__)
#include <io.h>
#endif

#define file_fopen  fopen
#define file_fclose fclose

/* binary file IO */
#define file_fread  fread

#define file_fseek _fseeki64
#else
#include <stdio.h>
#include <unistd.h>

#ifndef __USE_LARGEFILE64
#define __USE_LARGEFILE64
#endif

#ifndef __USE_LARGEFILE
#define __USE_LARGEFILE
#endif

#if defined __APPLE__ || defined (INTEL64) || defined(__USE_LARGEFILE64)
/* native fopen is 64-bits */
#define file_fopen fopen
#else
#define file_fopen fopen64
#endif

#define file_fclose  fclose

/* binary file IO */
#define file_fread  fread

#if defined unix || defined UNIX || defined __APPLE__
unsigned long long file_fseek(FILE *fd, long long position, int mode)
{
#if defined ANDROID
    return fseek(fd, (size_t)position, mode);
#else
#if defined __APPLE__ || defined INTEL64
    return fseeko(fd, (off_t)position, mode);
#else
    return fseeko64(fd, (__off64_t)position, mode);
#endif
#endif
}
#endif

#endif
/***********************/

struct BMPHeader
{
    // file header
    unsigned short  bfType;
    unsigned int    bfSize;
    unsigned short  bfReserved1;
    unsigned short  bfReserved2;
    unsigned int    bfOffBits;

    // image header
    unsigned int    biSize;
    unsigned int    biWidth;
    int             biHeight;
    unsigned short  biPlanes;
    unsigned short  biBitCount;
    unsigned int    biCompression;
    unsigned int    biSizeImage;
    unsigned int    biXPelsPerMeter;
    unsigned int    biYPelsPerMeter;
    unsigned int    biClrUsed;
    unsigned int    biClrImportant;
};

struct RGBquad
{
    unsigned char    rgbBlue;
    unsigned char    rgbGreen;
    unsigned char    rgbRed;
    unsigned char    rgbReserved;
};

Status BmpReadData(FILE *pFile, Ipp8u **pImg, IppiSize &imgSize, int &imgStep, int &numChannels)
{
    if(!pImg || !pFile)
        return STS_ERR_NULL_PTR;

    RGBquad      palette[256];
    BMPHeader    header;

    memset(&palette[0], 0, sizeof(palette));
    memset(&header, 0, sizeof(BMPHeader));

    // read header
    file_fread(&header.bfType, 1, sizeof(header.bfType), pFile);
    file_fread(&header.bfSize, 1, sizeof(header.bfSize), pFile);

    if(header.bfType != 0x4D42) /* 'MB' */
        return STS_ERR_FORMAT;

    file_fread(&header.bfReserved1, 1, sizeof(header.bfReserved1), pFile);
    file_fread(&header.bfReserved2, 1, sizeof(header.bfReserved2), pFile);
    file_fread(&header.bfOffBits, 1, sizeof(header.bfOffBits), pFile);

    file_fread(&header.biSize, 1, sizeof(header.biSize), pFile);
    file_fread(&header.biWidth, 1, sizeof(header.biWidth), pFile);
    file_fread(&header.biHeight, 1, sizeof(header.biHeight), pFile);
    file_fread(&header.biPlanes, 1, sizeof(header.biPlanes), pFile);
    file_fread(&header.biBitCount, 1, sizeof(header.biBitCount), pFile);

    if(header.biBitCount != 8 && header.biBitCount != 24 && header.biBitCount != 32)
        return STS_ERR_FAILED;

    file_fread(&header.biCompression, 1, sizeof(header.biCompression), pFile);

    switch(header.biCompression)
    {
    case 0L: //0L == BI_RGB
        break;

    default:
        return STS_ERR_FAILED;
    }

    file_fread(&header.biSizeImage, 1, sizeof(header.biSizeImage), pFile);
    file_fread(&header.biXPelsPerMeter, 1, sizeof(header.biXPelsPerMeter), pFile);
    file_fread(&header.biYPelsPerMeter, 1, sizeof(header.biYPelsPerMeter), pFile);
    file_fread(&header.biClrUsed, 1, sizeof(header.biClrUsed), pFile);
    file_fread(&header.biClrImportant, 1, sizeof(header.biClrImportant), pFile);

    if(header.biBitCount == 8)
        file_fread(&palette, 1, sizeof(RGBquad)*256, pFile);

    imgSize.width   = IPP_ABS(header.biWidth);
    imgSize.height  = IPP_ABS(header.biHeight);
    if(imgSize.width > MAX_WIDTH || imgSize.height > MAX_HEIGHT)
        return STS_ERR_UNSUPPORTED;
    numChannels     = header.biBitCount >> 3;
    imgStep         = alignSize(imgSize.width * numChannels, 32);

    if(numChannels != 1 && numChannels != 3)
        return STS_ERR_UNSUPPORTED;

    // read data
    if(!header.bfOffBits)
        return STS_ERR_FAILED;

    if(file_fseek(pFile, header.bfOffBits, SEEK_SET))
        return STS_ERR_FAILED;

    /* Alloc memory */
    *pImg = ippsMalloc_8u(imgStep * imgSize.height);

    if(*pImg == NULL)
        return STS_ERR_ALLOC;

    unsigned int iFileStep = alignSize(header.biWidth*(header.biBitCount >> 3), 4);

    if(0 < header.biHeight) // read bottom-up BMP
    {
        unsigned char *pPtr = *pImg + imgStep * (imgSize.height - 1);

        for(unsigned int i = 0; i < imgSize.height; i++)
        {
            if(iFileStep != file_fread((unsigned char*)(pPtr - i * imgStep), 1, iFileStep, pFile))
                return STS_ERR_NOT_ENOUGH_DATA;
        }
    }
    else // read up-bottom BMP
    {
        for(unsigned int i = 0; i < imgSize.height; i++)
        {
            if(iFileStep != file_fread(*pImg + i * imgStep, 1, iFileStep, pFile))
                return STS_ERR_NOT_ENOUGH_DATA;
        }
    }

    return STS_OK;
}

Status ReadFile(const char *pFileName, Ipp8u **pImg, IppiSize &imgSize, int &imgStep, int &numChannels)
{
    FILE *pFile = 0;
    Status status = STS_OK;

    // allocate source buffer and read from file
    pFile = file_fopen(pFileName, "rb");
    if(!pFile)
        return STS_ERR_FILE_OPEN;

    status = BmpReadData(pFile, pImg, imgSize, imgStep, numChannels);
    CHECK_STATUS_PRINT(status, "Image::Read()", GetBaseStatusString(status));

    file_fclose(pFile);

    return status;
}
