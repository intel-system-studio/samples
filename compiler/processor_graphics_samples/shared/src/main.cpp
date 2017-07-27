//==============================================================
//
// SAMPLE SOURCE CODE - SUBJECT TO THE TERMS OF SAMPLE CODE LICENSE AGREEMENT,
// http://software.intel.com/en-us/articles/intel-sample-source-code-license-agreement/
//
// Copyright 2016 Intel Corporation
//
// THIS FILE IS PROVIDED "AS IS" WITH NO WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT
// NOT LIMITED TO ANY IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE, NON-INFRINGEMENT OF INTELLECTUAL PROPERTY RIGHTS.
//
// =============================================================


#include "Workload.hpp"
#include "PerfTimer.h"
#include <float.h>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <vector>
#include <string.h>
#include <stdlib.h>

using namespace std;

struct WorkRecord
{
    const char* (*name)();
    Workload* (*create)();
};

typedef vector<WorkRecord> WorkRecords;
static WorkRecords* workrecords = 0;

static ofstream ofs;
static Properties props;

int Workload::registry (const char* (*name)(), Workload* (*create)())
{
    if (workrecords == 0) {
        workrecords = new WorkRecords();
    }

    int id = workrecords->size();
    WorkRecord tmp;
    tmp.name   = name,
    tmp.create = create;
    workrecords->push_back(tmp);
    return id;
}

static Workload* create (const char* name)
{
    for (WorkRecords::iterator it = workrecords->begin();
        it != workrecords->end(); ++it) {
#ifdef __linux__
        if (strcasecmp(it->name(), name) == 0)
#else
        if (_stricmp(it->name(), name) == 0)
#endif
        {
            return it->create();
        }
    }
    return 0;
}

bool Workload::setup (Properties* p)
{
    props = p;

    string s("offload");
    props->get("exec", s);
    if (s.compare("offload_off") == 0) {
        exmode = OFFLOADoff;
        cpu_share = 1.0;
        props->put("exec", "offload_off");
    }
    else if (s.compare("offload_on") == 0 ||
             s.compare("offload") == 0) {
        props->get("cpu_share", cpu_share = 0.0);
        if (cpu_share < 0.0) {
            cpu_share = 0.0;
        }

        if (cpu_share >= 1.0) {
            cpu_share = 1.0;
            exmode = OFFLOADoff;
            props->put("exec", "offload_off");
        }
        else {
            exmode = OFFLOADon;
            props->put("exec", "offload");
        }
    }
    else if (s.compare("cpu") == 0) {
        exmode = CPU;
        cpu_share = 1.0;
        props->put("exec", "cpu");
    }
    else {
        cerr << "invalid exec value: " << s << endl;
        return false;
    }

    props->put("cpu_share", cpu_share);

    if (!props->get("validate", verify = true)) {
        props->put("validate", verify);
    }

    cout<< "<config>" << endl;
    for (Properties::const_iterator it = props->begin(); it != props->end();
        ++it) {
        cout<< " " << it->first << " = " << it->second << endl;
    }
    cout << "</config>" << endl << endl;
    cout << "exec mode is " << (exmode == OFFLOADon ? "offload" :
        (exmode == OFFLOADoff ? "offload_off (no offload)" : "cpu")) << endl;
    return true;
}


typedef bool (Workload::*WorkloadPhase) ();

static bool work_call (Workload* work, WorkloadPhase phase)
{
    bool r = false;
    try {
        r = ((work)->*(phase))();
    }
    catch (char* msg) {
        cout << "*** exception in workload ***" << endl
             << msg << endl;
    }
    catch (...) {
        cout << "*** exception in workload ***" << endl;
    }
    return r;
}

static bool executor (Workload* work, bool guarded)
{
    bool r = false;
    if (work->exmode == Workload::CPU) {
        r = work_call(work, &Workload::execute_cpu);
    }
    else {
        r = work_call(work, &Workload::execute_offload);
    }

    return r;
}

static int execute (const char* name, int iterations)
{
    Workload* work = create(name);
    if (work == 0) {
        cout << "workload not found: " << name << endl;
        return 1;
    }

    cout << endl << "executing workload: " << name << endl << endl;

    if (!work->setup(&props)) {
        delete work;
        return 1;
    }

bool verbose, guarded;
    work->prop_get("verbose", verbose = true);
    work->prop_get("guarded", guarded = true);

    bool single_init, single_validate;
    work->prop_get("single.init",     single_init = true);
    work->prop_get("single.validate", single_validate = true);

    cout << endl
         << "execution started for " << iterations << " iterations"
         << ", validate is " << (work->verify ? "on" : "OFF")
         << ", single.init is " << (single_init ? "true" : "FALSE")
         << ", single.validate is " << (single_validate ? "TRUE" : "false")
         << endl;

    double* iter_times = new double[iterations];

    PerfTimer timer_total, timer_iter;
    timer_total.start();

    int it, err;
    bool go = true;
    for (it = 0, err = 0; go; ) {
        // initialization (call open)
        if (it == 0 || !single_init) {
            if (verbose) cout << "calling open ...";
            timer_iter.start();
            if (!work_call(work, &Workload::open)) {
                ++err;
                cout << "initialization error" << endl;
                break;
            }
            timer_iter.stop();
            if (verbose) cout << endl;
        }

        // execution (call executor)
        if (verbose) cout << "iteration #" << it << " ...";
        timer_iter.start();
        bool r = executor(work, guarded);
        timer_iter.stop();
        if (verbose) cout << endl;
        iter_times[it] = timer_iter.sec()*1000;

        if (!r) {
            ++err;
            cout << "execution failure" << endl;
        }
        else if (work->verify && (it == 0 || !single_validate)) {
            // validate (call validate)
            if (verbose) cout << "calling validate ...";
            if (!work_call(work, &Workload::validate)) {
                ++err;
                cout << " validation failure" << endl;
            }
            else {
                if (verbose) cout << " ok" << endl;
            }
        }

        go = (++it < iterations) && (err == 0);

        // de-initialization (call close)
        if (!go || !single_init) {
            if (verbose) cout << "calling close ...";
            timer_iter.start();
            work_call(work, &Workload::close);
            timer_iter.stop();
            if (verbose) cout << endl;
        }
    }

    double ms_total = 0,
           ms_min   = DBL_MAX;
    int it_min;
    for (int i = 0; i < it; ++i) {
        ms_total += iter_times[i];
        if (ms_min > iter_times[i]) {
            ms_min = iter_times[i];
            it_min = i;
        }
    }

    timer_total.stop();
    cout << setprecision(3) << fixed << "execution finished in " << ms_total
         << " ms" << ", total time " << timer_total.sec()/60.0 << "min"
         << endl;
    if (it > 1) {
        cout << "first iteration: " << iter_times[0] << "ms" << endl;
        cout << "best time per iteration: " << ms_min << "ms (iteration# "
        << it_min << ")" << endl;
        cout << "mean time: " << (ms_total - iter_times[0])/(it-1)
        << " ms (first iteration excluded)" << endl;
    }
    cout << endl;

    cout << (err == 0 ? "passed" : "FAILED") << endl;

    delete work;
    return err;
}

void cleanup ()
{
}

void usage ()
{
    cout << endl
         << "Options available:"                  << endl
         << "  workload=<name>"                   << endl
         << "  exec=offload|offload_off|cpu"      << endl
         << "  cpu_share=<0.0 .. 1.0>"            << endl
         << "  iterations=<count>"                << endl
         << "  validate=on|off"                   << endl
         << "  single.validate=off|on"            << endl
         << "  single.init=on|off"                << endl
         << "  logfile=<name>"                    << endl
         << "  guarded=on|off"                    << endl
         << "  verbose=on|off"                    << endl
         << endl
         << "Alternative values are separated with |, first alternative is "
         << "the default one"                     << endl
         << "For boolean values, true and on are the same, "
         << "so are false and off"                << endl;
}

int main(int argc, char *argv[])
{
    cout << "Workloads available:" << endl;
    for (WorkRecords::iterator it = workrecords->begin();
        it != workrecords->end(); ++it)
        cout << " " << it->name() << endl;

    bool ok = true;

    for (int i = 1; i < argc; ++i) {
        if (strchr(argv[i], '=') == 0) {
            ok &= props.read(argv[i]);
        }
        else {
            ok &= props.readln(argv[i]);
        }
    }

    if (!ok) {
        usage();
        return 1;
    }

    char* workname;
    string logfile;
    int iterations;

    props.get("workload", workname = strdup(workrecords->begin()->name()));
    props.get("logfile",  logfile);
    props.get("iterations", iterations = 10);

    if (workname == 0 || strlen(workname) == 0) {
        cout << "missing workload" << endl;
        return -1;
    }

    if (!logfile.empty()) {
        ofs.open(logfile.c_str(), ios_base::app);
        if (!ofs.good()) {
            cout << "failed to open output file: " << logfile << endl;
            return -1;
        }
    }

    if (atexit(cleanup) != 0) {
        cout << "FAILED atexit(cleanup)" << endl;
    }

    int errors = 0;

    if (strcmp(workname, "ALL") == 0) {
        for (WorkRecords::iterator it = workrecords->begin();
            it != workrecords->end(); ++it) {
            errors += execute(it->name(), iterations);
        }
    }
    else {
        errors = execute(workname, iterations);
    }

    if (workname) {
        free(workname);
    }

    ofs.close();

    return errors;
}
