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

#ifndef FLUID_ANIMATEAN_H
#define FLUID_ANIMATEAN_H

#define _USE_MATH_DEFINES
#include <cmath>
#include <string>
#include <cstddef>

#include "common.h"

// a simple x,y,z vector class that utilizes Array Notation
class Vec3AN {
public:
	// Add an extra float for so that vector math will line up on the cache line
    float vec[4];

    Vec3AN() {}
	Vec3AN(float _x, float _y, float _z)  {vec[0] = _x; vec[1] = _y; vec[2] = _z; vec[3] = 0;}
	Vec3AN(float v[3]) { vec[0:3] = v[:]; vec[3] = 0;}
	Vec3AN(const Vec3AN &v) { vec[:] = v.vec[:]; }

	float   GetLengthSq() const { return __sec_reduce_add(vec[:] * vec[:]); }
    float   GetLength() const   { return sqrtf(GetLengthSq()); }
    Vec3AN &  Normalize()       { return *this /= GetLength(); }

	Vec3AN &  operator =  (Vec3AN const &v) { vec[:] = v.vec[:]; return *this; } 
    Vec3AN &  operator += (Vec3AN const &v) { vec[:] = vec[:] + v.vec[:]; return *this; }
    Vec3AN &  operator -= (Vec3AN const &v) { vec[:] = vec[:] - v.vec[:]; return *this; }
    Vec3AN &  operator *= (float s)         { vec[:] = vec[:] * s; return *this; }
    Vec3AN &  operator /= (float s)         { vec[:] = vec[:] / s; return *this; }

    Vec3AN    operator + (Vec3AN const &v) const    { Vec3AN tmp(*this); tmp.vec[:] += v.vec[:]; return tmp; }
    Vec3AN    operator - (Vec3AN const &v) const    { Vec3AN tmp(*this); tmp.vec[:] -= v.vec[:]; return tmp; }
    Vec3AN    operator * (float s) const            { Vec3AN tmp(*this); tmp.vec[:] *= s; return tmp;}
    Vec3AN    operator / (float s) const            { Vec3AN tmp(*this); tmp.vec[:] /= s; return tmp; }
};

// Helper structure for padding calculation, not used directly by the program
struct Cell_auxAN {
  	Vec3AN p[c_particles_per_cell];
	Vec3AN hv[c_particles_per_cell];
	Vec3AN v[c_particles_per_cell];
	Vec3AN a[c_particles_per_cell];
	float density[c_particles_per_cell];
	Cell_auxAN *next;
	//dummy variable so we can reference the end of the payload data
	char padding;
};

#pragma warning(disable : 1875)
// Real Cell structure
// there is a current limitation of c_particles_per_cell particles per cell
// (this structure use to be a simple linked-list of particles but, due to
// improved cache locality, we get a huge performance increase by copying
// particles instead of referencing them)
struct CellAN {
  	Vec3AN p[c_particles_per_cell];
	Vec3AN hv[c_particles_per_cell];
	Vec3AN v[c_particles_per_cell];
	Vec3AN a[c_particles_per_cell];
	float density[c_particles_per_cell];
	CellAN *next;
	//padding to force cell size to a multiple of estimated cache line size
	char padding[c_cacheline_size - (offsetof(Cell_auxAN, padding) % c_cacheline_size)];
	CellAN() : next(NULL) {}
};

// gravity
const Vec3AN c_external_accelerationAN(0.f, -9.8f, 0.f);
// domain of sample -- particles stay within this min/max
const Vec3AN c_domain_minAN(-0.065f, -0.08f, -0.065f);
const Vec3AN c_domain_maxAN(0.065f, 0.1f, 0.065f);

void init_simAN(std::string fileName);
void advance_frameAN();
void advance_frame_cilkAN();
void end_simAN(std::string filename);

#endif // FLUID_ANIMATEAN_H
