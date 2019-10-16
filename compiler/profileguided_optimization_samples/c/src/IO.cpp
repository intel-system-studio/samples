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

#include "IO.h"
#include <fstream>
#include <iostream>
#include <cassert>

using namespace std;

int numParticles = 0;
float restParticlesPerMeter;
ifstream file;
ofstream file2;

// Opens the file of name fileName for reading
void open_read_file(string filename) {
	file.open(filename.c_str(), ios::binary);
    if (!file.is_open())
    {
        std::cout<<"Error opening file: "<<filename.c_str()<<std::endl;
        std::cout<<"Please try change the execution directory or input data path."<<std::endl;        
    }
    assert(file.is_open());
}

// Closes the read file
void close_read_file() {
	file.close();
}

// Get and return the restParticlePerMeter and numParticles
RPPM_and_numPart get_RPPM_and_numPart() {
	assert(file.is_open());
	file.read((char *)&restParticlesPerMeter, 4);
	file.read((char *)&numParticles, 4);
	RPPM_and_numPart ran;
	ran.restParticlesPerMeter = restParticlesPerMeter;
	ran.numParticles = numParticles;
	return ran;
}

// gets the data for one particle
pardata get_particle() {
	assert(file.is_open());
	pardata pd;
	file.read((char *)&pd.px, 4);
	file.read((char *)&pd.py, 4);
	file.read((char *)&pd.pz, 4);
	file.read((char *)&pd.hvx, 4);
	file.read((char *)&pd.hvy, 4);
	file.read((char *)&pd.hvz, 4);
	file.read((char *)&pd.vx, 4);
	file.read((char *)&pd.vy, 4);
	file.read((char *)&pd.vz, 4);
	
	return pd;
}
// Opens the file of name fileName for writing
void open_save_file(string filename) {
	file2.open(filename.c_str(), ios::binary);
	assert(file2.is_open());
}

// Closes the write file
void close_save_file() {
	file2.close();
}

// writes the restParticlesPerMeter and numParticles values
void save_RPPM_and_numPart() {
	assert(file2.is_open());
	file2.write((char *)&restParticlesPerMeter, 4);
	file2.write((char *)&numParticles, 4);
}

// saves a single particle to file
void save_particle(float px, float py, float pz, float hvx, float hvy, float hvz, float vx, float vy, float vz) {
	assert(file2.is_open());
	file2.write((char *)&px,  4);
	file2.write((char *)&py,  4);
	file2.write((char *)&pz,  4);
	file2.write((char *)&hvx, 4);
	file2.write((char *)&hvy, 4);
	file2.write((char *)&hvz, 4);
	file2.write((char *)&vx,  4);
	file2.write((char *)&vy,  4);
	file2.write((char *)&vz,  4);
}
