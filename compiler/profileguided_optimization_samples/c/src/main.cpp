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

#include "fluid_animate.h"
#include "timer.h"
#include <iostream>
#include <string>
#include <cstdlib>
#ifdef _WIN32
#include <Windows.h>
#endif

#ifdef __INTEL_COMPILER
#include "fluid_animateAN.h"
#endif

using namespace std;

#ifndef PERF_NUM
int option = -1;
#else
int option = 0;
#endif
string save_file_base;
void parse_args(int argc, char *argv[]) {
	for(int i=1; i<argc; ++i) {
		string arg = argv[i];
		if(i+1 < argc) {
			string arg2 = argv[i+1];
			if(arg == "-o" || arg == "-O") {
				if (arg2 == "h" || arg2 == "help") {
					cout << "This example will simulate the flow of a fluid by treating the fluid as a discrete set of particles and measuring the applied forces based on the proximity of the particles." << endl;
					cout << "For option:" << endl << "[-o 0|1|2|3|4]" << endl << "[0] all tests" << endl << "[1] serial/scalar" << endl ;
					cout << "To specify a file to save results:" << endl << "[-f <filename>]" << endl;
#ifdef _WIN32
					system("PAUSE");
#endif
					option=-2;
				}
				option = atoi(argv[i+1]);
			}
			else if (arg == "-f") {
				save_file_base = "../data/" + arg2;
			}
		}
	}
}

int main(int argc, char *argv[]) {
	const char *in_file = "../data/in_15K.fluid";
	int framenum = 2000;

	string save_file;
	parse_args(argc,argv);
	if (option == -2)
		return 0;

#ifndef __INTEL_COMPILER
#ifdef PERF_NUM
	double avg_time = 0;
	for(int i=0; i<5; ++i) {
#else
	cout << "This example will simulate the flow of a fluid by treating the fluid as a discrete set of particles and measuring the applied forces based on the proximity of the particles." << endl;;
#endif
	CUtilTimer timer;
    cout << endl << "Starting serial, scalar Fluid Animate..." << endl;
	init_sim(in_file);
	timer.start();
	for(int i=0; i<framenum; ++i)
		advance_frame();
	timer.stop();
	cout << "Calculation finished. Time taken is " << timer.get_time()*1000.0 << "ms" << endl;
	if(!save_file_base.empty()) {
		save_file.assign(save_file_base).append("_serial.fluid");
		cout << "Saving file \"" << save_file << "\"..." << endl;
	}
	end_sim(save_file);
#ifdef PERF_NUM
	avg_time += timer.get_time();
	}
	cout << "avg time: " << avg_time*1000.0/5 << "ms" << endl;
#endif
#else
#ifndef PERF_NUM
	// If no options are given, prompt user to choose an option
	if(option == -1) {
		cout << "This example will simulate the flow of a fluid by treating the fluid as a discrete set of particles and measuring the applied forces based on the proximity of the particles." << endl;
		cout << "[0] all tests" << endl << "[1] serial/scalar" << endl << "[2] serial/AN" << endl << "  > ";
		cin >> option;
	}
#endif // !PERF_NUM

	CUtilTimer timer;
	double serial_time, vec_time;

	switch (option) {
	case 0:
        cout << endl << "Running all tests" << endl;
#ifdef PERF_NUM
		double avg_time[4];
		avg_time[:] = 0.0;
		for(int i=0; i<5; ++i) {
#endif

    	cout << endl << "Starting serial, scalar Fluid Animate..." << endl;
		init_sim(in_file);
		timer.start();
		for(int i=0; i<framenum; ++i)
			advance_frame();
		timer.stop();
		serial_time = timer.get_time();
		cout << "Calculation finished. Time taken is " << serial_time*1000.0 << "ms" << endl;
		if(!save_file_base.empty()) {
			save_file.assign(save_file_base).append("_serial.fluid");
			cout << "Saving file \"" << save_file << "\"..." << endl;
		}
		end_sim(save_file);

    	cout << endl << "Starting serial, AN Fluid Animate..." << endl;
		init_simAN(in_file);
		timer.start();
		for(int i=0; i<framenum; ++i)
			advance_frameAN();
		timer.stop();
		vec_time = timer.get_time();
		cout << "Calculation finished. Time taken is " << vec_time*1000.0 << "ms" << endl;
		if(!save_file_base.empty()) {
			save_file.assign(save_file_base).append("_vec.fluid");
			cout << "Saving file \"" << save_file << "\"..." << endl;
		}
		end_simAN(save_file);

#ifdef PERF_NUM
		avg_time[0] += serial_time;
		avg_time[1] += vec_time;

		}
		cout << endl << "avg time: " << endl << avg_time[0]*1000.0/5 << endl << avg_time[1]*1000.0/5 << endl;
#endif
		break;

	case 1:
    	cout << endl << "Starting serial, scalar Fluid Animate..." << endl;
		init_sim(in_file);
		timer.start();
		for(int i=0; i<framenum; ++i)
			advance_frame();
		timer.stop();
		cout << "Calculation finished. Time taken is " << timer.get_time()*1000.0 << "ms" << endl;
		if(!save_file_base.empty()) {
			save_file.assign(save_file_base).append("_serial.fluid");
			cout << "Saving file \"" << save_file << "\"..." << endl;
		}
		end_sim(save_file);
		break;

	case 2:
		cout << endl << "Starting serial, AN Fluid Animate..." << endl;
		init_simAN(in_file);
		timer.start();
		for(int i=0; i<framenum; ++i)
			advance_frameAN();
		timer.stop();
		vec_time = timer.get_time();
		cout << "Calculation finished. Time taken is " << vec_time*1000.0 << "ms" << endl;
		if(!save_file_base.empty()) {
			save_file.assign(save_file_base).append("_vec.fluid");
			cout << "Saving file \"" << save_file << "\"..." << endl;
		}
		end_simAN(save_file);
		break;

	default:
        cout << "Please pick a valid option" << endl;
		break;
	}
#endif

#ifdef _WIN32
    system("PAUSE");
#endif
    return 0; 
}
