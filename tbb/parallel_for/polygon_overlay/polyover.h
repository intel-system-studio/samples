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

/*!
 * polyover.h : extern declarations for polyover.cpp
*/
#include "rpolygon.h"
#include "tbb/mutex.h"
#include "tbb/spin_mutex.h"

extern void OverlayOnePolygonWithMap(Polygon_map_t *resultMap, RPolygon *myPoly, Polygon_map_t  *map2, tbb::spin_mutex *rMutex);

extern void SerialOverlayMaps(Polygon_map_t **resultMap, Polygon_map_t *map1, Polygon_map_t *map2);

// extern void NaiveParallelOverlay(Polygon_map_t **result_map, Polygon_map_t *polymap1, Polygon_map_t *polymap2);
extern void NaiveParallelOverlay(Polygon_map_t *&result_map, Polygon_map_t &polymap1, Polygon_map_t &polymap2);

extern void SplitParallelOverlay(Polygon_map_t **result_map, Polygon_map_t *polymap1, Polygon_map_t *polymap2);
extern void SplitParallelOverlayCV(concurrent_Polygon_map_t **result_map, Polygon_map_t *polymap1, Polygon_map_t *polymap2);
extern void SplitParallelOverlayETS(ETS_Polygon_map_t **result_map, Polygon_map_t *polymap1, Polygon_map_t *polymap2);

extern void CheckPolygonMap(Polygon_map_t *checkMap);
extern bool ComparePolygonMaps(Polygon_map_t *map1, Polygon_map_t *map2);

