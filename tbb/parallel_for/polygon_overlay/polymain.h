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

#include "pover_global.h"  // for declaration of DEFINE and INIT

DEFINE Polygon_map_t *gPolymap1 INIT(0);
DEFINE Polygon_map_t *gPolymap2 INIT(0);
DEFINE Polygon_map_t *gResultMap INIT(0);

extern void Usage(int argc, char **argv);

extern bool ParseCmdLine(int argc, char **argv );

extern bool GenerateMap(Polygon_map_t **newMap, int xSize, int ySize, int gNPolygons, colorcomp_t maxR, colorcomp_t maxG, colorcomp_t maxB);

extern bool PolygonsOverlap(RPolygon *p1, RPolygon *p2, int &xl, int &yl, int &xh, int &yh);

extern void CheckPolygonMap(Polygon_map_t *checkMap);

extern bool CompOnePolygon(RPolygon *p1, RPolygon *p2);

extern bool PolygonsEqual(RPolygon *p1, RPolygon *p2);

extern bool ComparePolygonMaps(Polygon_map_t *map1, Polygon_map_t *map2);

extern void SetRandomSeed(int newSeed);

extern int NextRan(int n);
