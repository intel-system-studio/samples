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

//
//  Created by Xcode* 4.3.2
//

#import <Cocoa/Cocoa.h>

@interface guiAppDelegate : NSObject <NSApplicationDelegate>{
    __unsafe_unretained NSWindow *_window;
}

@property (assign) IBOutlet NSWindow *window;

- (BOOL) applicationShouldTerminateAfterLastWindowClosed:(NSApplication *) sender;

@end
