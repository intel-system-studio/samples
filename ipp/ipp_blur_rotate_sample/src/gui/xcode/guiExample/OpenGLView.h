/*
//==============================================================
//
// SAMPLE SOURCE CODE - SUBJECT TO THE TERMS OF SAMPLE CODE LICENSE AGREEMENT,
// http://software.intel.com/en-us/articles/intel-sample-source-code-license-agreement/
//
// Copyright 2005-2016 Intel Corporation
//
// THIS FILE IS PROVIDED "AS IS" WITH NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT
// NOT LIMITED TO ANY IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE, NON-INFRINGEMENT OF INTELLECTUAL PROPERTY RIGHTS.
//
// =============================================================
*/

#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>

@interface OpenGLView : NSOpenGLView{
    NSTimer *timer;
}

@property (nonatomic,retain) NSTimer *timer;

- (void) drawRect:(NSRect)start;
- (void) mouseDown:(NSEvent *)theEvent;
- (void) keyDown:(NSEvent *)theEvent;
- (BOOL) acceptsFirstResponder;
- (void) viewDidEndLiveResize;

@end
