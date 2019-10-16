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

#ifndef IO_H
#define IO_H

#include <string>

typedef struct {
	float px, py, pz,
	hvx, hvy, hvz,
	vx, vy, vz;
} pardata;

typedef struct {
	float restParticlesPerMeter;
	int numParticles;
} RPPM_and_numPart;

void open_read_file(std::string filename);
void close_read_file();
RPPM_and_numPart get_RPPM_and_numPart();
pardata get_particle();

void open_save_file(std::string filename);
void close_save_file();
void save_RPPM_and_numPart();
void save_particle(float px, float py, float pz, float hvx, float hvy, float hvz, float vx, float vy, float vz);

#endif // IO_H
