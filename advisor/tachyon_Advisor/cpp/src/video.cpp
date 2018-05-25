//=======================================================================
//
// SAMPLE SOURCE CODE - SUBJECT TO THE TERMS OF END-USER LICENSE AGREEMENT FOR
// INTEL(R) ADVISOR XE.
//
// Copyright (C) 2014 Intel Corporation. All rights reserved
//
// THIS FILE IS PROVIDED "AS IS" WITH NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT
// NOT LIMITED TO ANY IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE, NON-INFRINGEMENT OF INTELLECTUAL PROPERTY RIGHTS.
//
// ========================================================================


/*
    The original source for this example is
    Copyright (c) 1994-2008 John E. Stone
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:
    1. Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.
    3. The name of the author may not be used to endorse or promote products
       derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
    OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
    OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
    HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
    LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
    OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
    SUCH DAMAGE.
*/

/*
 * video.c - routines for putting pixels on a screen if one is available.
 *
 *  $Id: video.cpp,v 1.20 2007-02-28 18:35:22  Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VIDEO_WINMAIN_ARGS
#include "types.h"
#include "api.h"       /* The ray tracing library API */
#include "getargs.h"   /* command line argument/option parsing */
#include "parse.h"     /* Support for my own file format */
#include "ui.h"
#include "util.h"
#include "video.h"

static SceneHandle global_scene;
static int global_xsize;     /*  size of graphic image rendered in window (from hres, vres)  */
static int global_ysize;
static int global_xwinsize;  /*  size of window (may be larger than above)  */
static int global_ywinsize;
static char *global_window_title;
static bool global_usegraphics;

static char *window_title_string (int argc, char **argv)
{
    int i;
    char *name;

    name = (char *) malloc (8192);
    
    if(strrchr(argv[0], '\\')) strcpy (name, strrchr(argv[0], '\\')+1);
    else if(strrchr(argv[0], '/')) strcpy (name, strrchr(argv[0], '/')+1);
    else strcpy (name, *argv[0]?argv[0]:"Tachyon");
    for (i = 1; i < argc; i++) {
        strcat (name, " ");
        strcat (name, argv[i]);
    }
#ifdef _DEBUG
    strcat (name, " (DEBUG BUILD)");
#endif
    return name;
}

static int main_init_parts (int argc, char **argv)
{
    int rc;
    argoptions opt;
    char * filename;

    global_window_title = window_title_string (argc, argv);

    global_scene = rt_newscene();

    rt_initialize(&argc, &argv);

    if ((rc = getargs(argc, argv, &opt)) == -1) {
#if _WIN32||_WIN64
        rt_sleep(10000);
#endif
        exit(rc);
    }

#ifdef DEFAULT_MODELFILE
#if  _WIN32||_WIN64
#define _GLUE_FILENAME(x) "..\\dat\\" #x
#else
#define _GLUE_FILENAME(x) #x
#endif
#define GLUE_FILENAME(x) _GLUE_FILENAME(x)
    if(opt.foundfilename == -1)
        filename = GLUE_FILENAME(DEFAULT_MODELFILE);
    else
#endif//DEFAULT_MODELFILE
        filename = opt.filename;

    rc = readmodel(filename, global_scene);

    if (rc != 0) {
        fprintf(stderr, "Parser returned a non-zero error code reading %s\n", filename);
        fprintf(stderr, "Aborting Render...\n");
        rt_finalize();
        return -1;
    }

    /* process command line overrides */
    useoptions(&opt, global_scene);

    // need these early for create_graphics_window() so grab these here...
    scenedef *scene = (scenedef *) global_scene;
    global_xsize = scene->hres;
    global_ysize = scene->vres;
    global_xwinsize = global_xsize;
    global_ywinsize = global_ysize;  // add some here to leave extra blank space on bottom for status etc.
    global_usegraphics = (scene->displaymode == RT_DISPLAY_ENABLED);

    return 0;
}

class tachyon_video : public video
{
    void on_process()
    {
        char buf[128];
        flt runtime;
        timerstart();
        rt_renderscene(global_scene);
        timerstop();
        runtime=timertime();
        sprintf(buf, "\nCPU Time: %.3f seconds.", runtime);
        rt_ui_message(MSG_0, buf); buf[0] = ' ';
        strcat(global_window_title, buf);
        title = global_window_title; updating = true;
        show_title();
        rt_finalize();
    }
    void on_key(int key) {
        key &= 0xff; if(key == 27) running = false;
    }
};
class video *video = 0;

void rt_finalize(void) {
    timerstart();
    if(global_usegraphics)
        do { rt_sleep(10); timerstop(); }
        while(timertime() < 10 && video->next_frame());
#ifdef _WINDOWS
    else rt_sleep(10000);
#endif
}

int main (int argc, char **argv)
{
    int rc;

    tachyon_video tachyon;
    tachyon.threaded = true;
    tachyon.init_console();

    rc = main_init_parts (argc, argv);
    if (rc) return rc;

    tachyon.title = global_window_title;
    tachyon.updating = global_usegraphics;
    // always using window even if(!global_usegraphics)
    global_usegraphics = 
        tachyon.init_window(global_xwinsize, global_ywinsize);
    if(!tachyon.running)
        return -1;

    video = &tachyon;
    tachyon.main_loop();

    return 0;
}
