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

//
//  Created by Xcode* 4.3.2
//

#import "guiAppDelegate.h"
#import <pthread.h>

@implementation guiAppDelegate

@synthesize window = _window;

//declared in macvideo.cpp file
extern int g_sizex, g_sizey;

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    // Insert code here to initialize your application
    NSRect windowSize;
    windowSize.size.height = g_sizey;
    windowSize.size.width = g_sizex;
    windowSize.origin=_window.frame.origin;
    [_window setFrame:windowSize display:YES];

}

- (BOOL) applicationShouldTerminateAfterLastWindowClosed:(NSApplication *) sender
{
    return YES;
}

@end
