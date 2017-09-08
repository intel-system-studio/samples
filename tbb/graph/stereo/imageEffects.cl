/*
    ==============================================================

    SAMPLE SOURCE CODE - SUBJECT TO THE TERMS OF SAMPLE CODE LICENSE AGREEMENT,
    http://software.intel.com/en-us/articles/intel-sample-source-code-license-agreement/

    Copyright (C) Intel Corporation

    THIS FILE IS PROVIDED "AS IS" WITH NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT
    NOT LIMITED TO ANY IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
    PURPOSE, NON-INFRINGEMENT OF INTELLECTUAL PROPERTY RIGHTS.

    =============================================================
*/

__constant int redChannelOffset = 0;
__constant int greenChannelOffset = 1;
__constant int blueChannelOffset = 2;
__constant int channelsPerPixel = 4;
__constant int channelIncreaseValue = 10;

__kernel void mergeImages( __global uchar* bufferLeft, __global uchar* bufferRight, uint width) {
    const int indexWidth = get_global_id(0);
    const int indexHeight = get_global_id(1);

    const int pixelIndex = channelsPerPixel * width * indexHeight + channelsPerPixel * indexWidth;
    const int pixelGreenChannelIndex = pixelIndex + greenChannelOffset;
    const int pixelBlueChannelIndex = pixelIndex + blueChannelOffset;

    bufferLeft[pixelGreenChannelIndex] = (bufferRight[pixelGreenChannelIndex] + bufferLeft[pixelGreenChannelIndex]) / 2;
    bufferLeft[pixelBlueChannelIndex] = bufferRight[pixelBlueChannelIndex];
}

__kernel void applyLeftImageEffect( __global uchar* bufferLeft, uint width) {
    const int indexWidth = get_global_id(0);
    const int indexHeight = get_global_id(1);

    const int pixelRedChannelIndex = channelsPerPixel * width * indexHeight + channelsPerPixel * indexWidth + redChannelOffset;

    bufferLeft[pixelRedChannelIndex] += channelIncreaseValue;
}

__kernel void applyRightImageEffect( __global uchar* bufferRight, uint width) {
    const int indexWidth = get_global_id(0);
    const int indexHeight = get_global_id(1);

    const int pixelBlueChannelIndex = channelsPerPixel * width * indexHeight + channelsPerPixel * indexWidth + blueChannelOffset;

    bufferRight[pixelBlueChannelIndex] += channelIncreaseValue;

}
