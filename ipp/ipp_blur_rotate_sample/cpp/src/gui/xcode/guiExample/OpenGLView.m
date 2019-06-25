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

#import "OpenGLView.h"
#import <OpenGL/gl.h>
#import "guiAppDelegate.h"

// defined in macvideo.cpp
extern char* window_title;
extern int cocoa_update;
extern int g_sizex, g_sizey;
extern unsigned int *g_pImg;
void on_mouse_func(int x, int y, int k);
void on_key_func(int x);

@implementation OpenGLView

@synthesize timer;

- (void) drawRect:(NSRect)start
{
    glWindowPos2i(0, (int)self.visibleRect.size.height);
    glPixelZoom( (float)self.visibleRect.size.width /(float)g_sizex,
                -(float)self.visibleRect.size.height/(float)g_sizey);
    glDrawPixels(g_sizex, g_sizey, GL_BGRA_EXT, GL_UNSIGNED_INT_8_8_8_8_REV, g_pImg);
    glFlush();

    timer = [NSTimer scheduledTimerWithTimeInterval:0.03 target:self selector:@selector(update_window) userInfo:nil repeats:YES];
}

-(void) update_window{
    if( cocoa_update ) [self setNeedsDisplay:YES]; // TODO also clear cocoa_update?
    if( window_title ) [_window setTitle:[NSString stringWithFormat:@"%s", window_title]];
}

-(void) keyDown:(NSEvent *)theEvent{
    on_key_func([theEvent.characters characterAtIndex:0]);
}

-(void) mouseDown:(NSEvent *)theEvent{
    // mouse event for seismic and fractal
    NSPoint point= theEvent.locationInWindow;
    const int x = (int)point.x;
    const int y = (int)point.y;
    NSRect rect = self.visibleRect;
    on_mouse_func(x*g_sizex/(int)rect.size.width,((int)rect.size.height-y)*g_sizey/(int)rect.size.height,1);
    [self setNeedsDisplay:YES];
}

- (BOOL) acceptsFirstResponder
{
    return YES;
}

- (void) rightMouseDown:(NSEvent *)theEvent
{
    return;
}

-(void) viewDidEndLiveResize
{
    NSRect rect = self.visibleRect;
    const int x=(int)rect.size.width;
    const int y=(int)rect.size.height;
    [_window setTitle:[NSString stringWithFormat:@"X=%d Y=%d", x,y]];
}

@end
