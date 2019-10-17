//==============================================================
//
// SAMPLE SOURCE CODE - SUBJECT TO THE TERMS OF SAMPLE CODE LICENSE AGREEMENT,
// http://software.intel.com/en-us/articles/intel-sample-source-code-license-agreement/
//
// Copyright Intel Corporation
//
// THIS FILE IS PROVIDED "AS IS" WITH NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT
// NOT LIMITED TO ANY IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE, NON-INFRINGEMENT OF INTELLECTUAL PROPERTY RIGHTS.
//
// =============================================================
// Code originally written by Richard O. Lee
// Modified by Christian Bienia and Christian Fensch

#ifndef FLUID_ANIMATE_H
#define FLUID_ANIMATE_H

#define _USE_MATH_DEFINES
#include <cmath>
#include <string>
#include <cstddef>

#include "common.h"

// A simple x,y,z vector
class Vec3 {
public:
    float x, y, z;

    Vec3() {}
    Vec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}

    float   GetLengthSq() const         { return x*x + y*y + z*z; }
    float   GetLength() const           { return sqrtf(GetLengthSq()); }

	Vec3 &  operator =  (Vec3 const &v) { x =  v.x;  y =  v.y; z =  v.z; return *this; }
    Vec3 &  operator += (Vec3 const &v) { x += v.x;  y += v.y; z += v.z; return *this; }
    Vec3 &  operator -= (Vec3 const &v) { x -= v.x;  y -= v.y; z -= v.z; return *this; }
    Vec3 &  operator *= (float s)       { x *= s;  y *= s; z *= s; return *this; }
    Vec3 &  operator /= (float s)       { x /= s;  y /= s; z /= s; return *this; }

    Vec3    operator + (Vec3 const &v) const    { return Vec3(x+v.x, y+v.y, z+v.z); }
    Vec3    operator - (Vec3 const &v) const    { return Vec3(x-v.x, y-v.y, z-v.z); }
    Vec3    operator * (float s) const          { return Vec3(x*s, y*s, z*s); }
    Vec3    operator / (float s) const          { return Vec3(x/s, y/s, z/s); }
};

// Helper structure for padding calculation, not used directly by the program
struct Cell_aux {
  	Vec3 p[c_particles_per_cell];
	Vec3 hv[c_particles_per_cell];
	Vec3 v[c_particles_per_cell];
	Vec3 a[c_particles_per_cell];
	float density[c_particles_per_cell];
	Cell_aux *next;
	//dummy variable so we can reference the end of the payload data
	char padding;
};

#ifndef _WIN32
#pragma warning(disable : 1875)
#endif
// Real Cell structure
// there is a current limitation of c_particles_per_cell particles per cell
// (this structure use to be a simple linked-list of particles but, due to
// improved cache locality, we get a huge performance increase by copying
// particles instead of referencing them)
struct Cell {
  	Vec3 p[c_particles_per_cell];
	Vec3 hv[c_particles_per_cell];
	Vec3 v[c_particles_per_cell];
	Vec3 a[c_particles_per_cell];
	float density[c_particles_per_cell];
	Cell *next;
	//padding to force cell size to a multiple of estimated cache line size
	char padding[c_cacheline_size - (offsetof(Cell_aux, padding) % c_cacheline_size)];
	Cell() : next(NULL) {}
};

// gravity
const Vec3 c_external_acceleration(0.f, -9.8f, 0.f);
// domain of sample -- particles stay within this min/max
const Vec3 c_domain_min(-0.065f, -0.08f, -0.065f);
const Vec3 c_domain_max(0.065f, 0.1f, 0.065f);

// functions to be accessed in main.cpp
void init_sim(std::string fileName);
void advance_frame();
void advance_frame_cilk();
void end_sim(std::string filename);


#endif
