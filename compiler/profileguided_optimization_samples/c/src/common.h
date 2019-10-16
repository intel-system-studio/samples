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

#ifndef COMMON_H
#define COMMON_H

const int c_cacheline_size = 64;
const int c_particles_per_cell = 16;

const float c_kernel_radius_multiplier = 1.695f;
const float c_time_step = 0.001f;
const float c_double_rest_density = 2000.f;
const float c_stiffness_pressure = 3.f;
const float c_stiffness_collisions = 30000.f;
const float c_par_size = 0.0002f;
const float c_epsilon = 1e-10f;
const float c_damping = 128.f;
const float c_viscosity = 0.4f;

#endif // COMMON_H
