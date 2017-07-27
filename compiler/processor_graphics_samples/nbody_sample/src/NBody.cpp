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


/*
 * NBody.cpp
 * Contains 3 version:
 * - NBody: Simple using SIMD enabled function and outer loop vectorization
 * - NBodyLocals: Optimized by caching data blocks in GRF arrays
 * - NBodyLocals_AOS: Similarly optimized version but operating on
 *   arrays-of-structs as input and output data
 * Supports mixed CPU-GPU execution (task sharing)
 */

#include <mathimf.h>
#include "Workload.hpp"
#include "GFXVector.hpp"
#include "Util.hpp"
#include <stdlib.h>
#include <iostream>

using namespace std;

template <class T>
struct NVect
{
    T * v;

    NVect (int n)        :v((T*)w_malloc_check(sizeof(T) * n)) {}
    ~NVect ()            {w_free(v);}
    operator T*    ()    {return v;}
};

template <class T>
struct RV
{
    const int N;
    NVect<T> rx, ry, rz;
    NVect<T> vx, vy, vz;

    RV (int n)            :N(n), rx(n), ry(n), rz(n), vx(n), vy(n), vz(n) {}
};

template <class FTYPE>
class NBody : public Workload
{
protected:

    static int id;

    static const char* name ();
    static Workload* create () {
#ifdef __INTEL_OFFLOAD
        cout << endl
             << "This program is built with __INTEL_OFFLOAD," << endl
             << "so it will execute with offload to Intel graphics or not "
             << "depending on \"exec\" mode (cpu|offload)." << endl;
#else
        cout << endl
             << "This program is built without __INTEL_OFFLOAD," << endl
             << "so it will execute without offload (on CPU) regardless of "
             << "other options." << endl;
#endif
        return new NBody();
    }

public:

    /*virtual*/ bool open ();
    /*virtual*/ bool execute_offload (int do_offload);
    /*virtual*/ bool execute_cpu ();
    /*virtual*/ bool validate ();
    /*virtual*/ bool close ();

protected:

    int n;
    FTYPE max_error, epsilon, time_step;
    NVect<FTYPE> * masses;
    RV<FTYPE> * init, * out;

    void check (FTYPE a, FTYPE b, int i, const char* msg, int& errs,
        FloatError&);
    void execute_serial   (RV<FTYPE> * );
    virtual void execute_part(int do_offload, int offset, int size);
};

#define TILE_j 32
#define TILE_i 32

template <class FTYPE>
class NBodyLocals : public NBody<FTYPE>
{
protected:

    static int id;

    static const char* name ();
    static Workload* create ()        {
#ifdef __INTEL_OFFLOAD
        cout << endl
             << "This program is built with __INTEL_OFFLOAD," << endl
             << "so it will execute with offload to Intel graphics or not "
             << "depending on \"exec\" mode (cpu|offload)." << endl;
#else
        cout << endl
             << "This program is built without __INTEL_OFFLOAD," << endl
             << "so it will execute without offload (on CPU) regardless of "
             << "other options." << endl;
#endif
        return new NBodyLocals();
    }

    virtual bool open ();

    virtual void execute_part(int do_offload, int offset, int size);

};

template <class FTYPE>
class NBodyLocals_AOS : public NBodyLocals<FTYPE>
{
protected:
    typedef GFXVector4<FTYPE> FTYPE4;

    static int id;

    FTYPE4 * init_pos4, * init_vel4, * out_pos4, * out_vel4;

    static const char* name ();
    static Workload* create ()        {
#ifdef __INTEL_OFFLOAD
        cout << endl
             << "This program is built with __INTEL_OFFLOAD," << endl
             << "so it will execute with offload to Intel graphics or not "
             << "depending on \"exec\" mode (cpu|offload)." << endl;
#else
        cout << endl
             << "This program is built without __INTEL_OFFLOAD," << endl
             << "so it will execute without offload (on CPU) regardless of "
             << "other options." << endl;
#endif
        return new NBodyLocals_AOS();
    }

    virtual bool open ();
    virtual bool close ();
    virtual bool validate ();

    virtual void execute_part(int do_offload, int offset, int size);
};

const char* NBody<float>::name ()        {return "nbody";}
int NBody<float>::id = Workload::registry(NBody<float>::name,
    NBody<float>::create);

const char* NBodyLocals<float>::name ()  {return "nbodylocals";}
int NBodyLocals<float>::id = Workload::registry(NBodyLocals<float>::name,
    NBodyLocals<float>::create);

const char* NBodyLocals_AOS<float>::name ()        {return "nbodylocals_aos";}
int NBodyLocals_AOS<float>::id = Workload::registry(
    NBodyLocals_AOS<float>::name, NBodyLocals_AOS<float>::create);

template <class FTYPE>
static FTYPE randf (FTYPE fMin, FTYPE fMax)
{
    rand();
    return fMin + (FTYPE) (rand() % (int)(fMax - fMin));
}

template <class FTYPE>
bool NBody<FTYPE>::open ()
{
    props->get("n", n = 16384);
    prop_get("max_error", max_error = 0.03f);
    epsilon =  0.0001f;
    time_step = 0.5f;

    cout << endl
          << "n = "         << n << endl
          << "max_error = " << max_error << endl;

    masses = new NVect<FTYPE>(n);
    init   = new RV<FTYPE>(n);
    out    = new RV<FTYPE>(n);

    FTYPE fMinMass     = 10.0f, fMaxMass     = 1000.0f,
          fMinPosition = 1.0f,  fMaxPosition = 10.0f,
          fMinVelocity = 1.0f,  fMaxVelocity = 10.0f;

    srand(2010);
    for (int i = 0; i < n; ++i)
    {
        init->rx[i]  = randf(fMinPosition, fMaxPosition);
        init->ry[i]  = randf(fMinPosition, fMaxPosition);
        init->rz[i]  = randf(fMinPosition, fMaxPosition);
        (*masses)[i] = randf(fMinMass,     fMaxMass);
        init->vx[i]  = randf(fMinVelocity, fMaxVelocity);
        init->vy[i]  = randf(fMinVelocity, fMaxVelocity);
        init->vz[i]  = randf(fMinVelocity, fMaxVelocity);
    }

    return true;
}

template <class FTYPE>
bool NBodyLocals<FTYPE>::open ()
{
    if (!NBody<FTYPE>::open()) {
        return false;
    }
    if (this->init->N % TILE_i != 0 || this->init->N % TILE_j != 0) {
        cout << "For tiled implementation n must be devisible by " << TILE_i;
        if (TILE_i != TILE_j) {
            cout << " and " << TILE_j;
        }
        cout << endl;
        return false;
    }

    return true;
}

template <class FTYPE>
void NBody<FTYPE>::check (FTYPE a, FTYPE b, int i, const char* msg, int& errs,
    FloatError& err)
{
    if (err.errorf(a, b)) {
        if (++errs < 10) {
            cout << " body# " << i << ", " << msg << ": " << a << " != " << b
                 << endl;
        }
    }
}

template <class FTYPE>
bool NBody<FTYPE>::close ()
{
    delete masses;
    delete out;

    return true;
}

template <class FTYPE>
bool NBody<FTYPE>::validate ()
{
    int errs = 0;
    // cast to float - temporary workaround
    FloatError err_r((float)max_error), err_v((float)max_error);

    RV<FTYPE> * gold = new RV<FTYPE>(n);
    execute_serial(gold);

    for (int i = 0; i < n; ++i) {
        check(out->rx[i], gold->rx[i], i, "rx", errs, err_r);
        check(out->ry[i], gold->ry[i], i, "ry", errs, err_r);
        check(out->rz[i], gold->rz[i], i, "rz", errs, err_r);
        check(out->vx[i], gold->vx[i], i, "vx", errs, err_v);
        check(out->vy[i], gold->vy[i], i, "vy", errs, err_v);
        check(out->vz[i], gold->vz[i], i, "vz", errs, err_v);
    }

    if (errs != 0) {
        cout << "total errors: " << errs
             << " (max error for r=" << err_r.max_error << ", v="
             << err_v.max_error << ")" << endl;
    }

    delete gold;

    return errs == 0;
}

template <class FTYPE>
bool NBodyLocals_AOS<FTYPE>::open ()
{
    if (!NBodyLocals<FTYPE>::open()) {
        return false;
    }
    init_pos4 = (FTYPE4*)w_malloc_check(this->init->N * sizeof(FTYPE4));
    init_vel4 = (FTYPE4*)w_malloc_check(this->init->N * sizeof(FTYPE4));
    out_pos4 = (FTYPE4*)w_malloc_check(this->init->N * sizeof(FTYPE4));
    out_vel4 = (FTYPE4*)w_malloc_check(this->init->N * sizeof(FTYPE4));

    SOA2AOS(init_pos4, this->init->rx, this->init->ry, this->init->rz,
            *this->masses, this->init->N);
    SOA2AOS(init_vel4, this->init->vx, this->init->vy, this->init->vz,
            this->init->N);
    return true;
}

template <class FTYPE>
bool NBodyLocals_AOS<FTYPE>::close ()
{
    w_free(init_pos4);
    w_free(init_vel4);
    w_free(out_pos4);
    w_free(out_vel4);
    return NBodyLocals<FTYPE>::close();
}

template <class FTYPE>
bool NBodyLocals_AOS<FTYPE>::validate ()
{
    AOS2SOA(this->out->rx, this->out->ry, this->out->rz,
            out_pos4, this->init->N);
    AOS2SOA(this->out->vx, this->out->vy, this->out->vz,
            out_vel4, this->init->N);
    return NBodyLocals<FTYPE>::validate();
}

template <class FTYPE>
void NBody<FTYPE>::execute_serial (RV<FTYPE>* out)
{
    FTYPE * mass = *masses;

    FTYPE * init_pos_x = init->rx, * init_pos_y = init->ry, * init_pos_z =
        init->rz;
    FTYPE * init_vec_x = init->vx, * init_vec_y = init->vy, * init_vec_z =
        init->vz;

    FTYPE * test_pos_x = out->rx, * test_pos_y = out->ry, * test_pos_z =
        out->rz;
    FTYPE * test_vec_x = out->vx, * test_vec_y = out->vy, * test_vec_z =
        out->vz;

    for (int i = 0; i < n; ++i) {
        FTYPE ax = 0, ay = 0, az = 0;

        for (int j = 0; j < n; ++j) {
            FTYPE dx = init_pos_x[j] - init_pos_x[i],
                  dy = init_pos_y[j] - init_pos_y[i],
                  dz = init_pos_z[j] - init_pos_z[i],
                  distanceInv = 1.0f/sqrt(dx*dx + dy*dy + dz*dz + epsilon),
                  K = mass[j]*distanceInv*distanceInv*distanceInv;

            ax += K*dx;
            ay += K*dy;
            az += K*dz;
        }

        test_vec_x[i] = init_vec_x[i] + ax*time_step;
        test_vec_y[i] = init_vec_y[i] + ay*time_step;
        test_vec_z[i] = init_vec_z[i] + az*time_step;

        test_pos_x[i] = init_pos_x[i] + init_vec_x[i]*time_step +
            ax*time_step*time_step/2.0f;
        test_pos_y[i] = init_pos_y[i] + init_vec_y[i]*time_step +
            ay*time_step*time_step/2.0f;
        test_pos_z[i] = init_pos_z[i] + init_vec_z[i]*time_step +
            az*time_step*time_step/2.0f;
    }
}

template <class FTYPE>
bool NBody<FTYPE>::execute_cpu ()
{
    FTYPE* mass = *masses;

    FTYPE * init_pos_x = init->rx, * init_pos_y = init->ry, * init_pos_z =
        init->rz;
    FTYPE * init_vec_x = init->vx, * init_vec_y = init->vy, * init_vec_z =
        init->vz;

    FTYPE * test_pos_x = out->rx, * test_pos_y = out->ry, * test_pos_z =
        out->rz;
    FTYPE * test_vec_x = out->vx, * test_vec_y = out->vy, * test_vec_z =
        out->vz;

    _Cilk_for (int i = 0; i < n; ++i) {
        FTYPE ax = 0, ay = 0, az = 0;
        #pragma simd
        for (int j = 0; j < n; ++j) {
            FTYPE dx = init_pos_x[j] - init_pos_x[i],
                  dy = init_pos_y[j] - init_pos_y[i],
                  dz = init_pos_z[j] - init_pos_z[i],
                  distanceInv = 1.0f/sqrt(dx*dx + dy*dy + dz*dz + epsilon),
                  K = mass[j]*distanceInv*distanceInv*distanceInv;

            ax += K*dx;
            ay += K*dy;
            az += K*dz;
        }

        test_vec_x[i] = init_vec_x[i] + ax*time_step;
        test_vec_y[i] = init_vec_y[i] + ay*time_step;
        test_vec_z[i] = init_vec_z[i] + az*time_step;

        test_pos_x[i] = init_pos_x[i] + init_vec_x[i]*time_step +
            ax*time_step*time_step/2.0f;
        test_pos_y[i] = init_pos_y[i] + init_vec_y[i]*time_step +
            ay*time_step*time_step/2.0f;
        test_pos_z[i] = init_pos_z[i] + init_vec_z[i]*time_step +
            az*time_step*time_step/2.0f;
    }
    return true;
}

template <class FTYPE>
bool NBody<FTYPE>::execute_offload(int do_offload)
{
    int n = init->N;
    int cpuShare = get_cpu_share(n);
    if (cpuShare > 0 && cpuShare < n) {
        _Cilk_spawn execute_part(0, n - cpuShare, cpuShare);
        execute_part(1, 0, n - cpuShare);
    }
    else {
        execute_part(do_offload, 0, n);
    }
    return true;
}

template <class FTYPE>
__declspec(target(gfx))
#ifdef __GFX__
__declspec(vector(linear(i),\
    uniform(body_count, mass, init_pos_x, init_pos_y, init_pos_z, init_vec_x,\
        init_vec_y, init_vec_z),\
    uniform(test_pos_x, test_pos_y, test_pos_z, test_vec_x, test_vec_y,\
        test_vec_z),\
    uniform(time_step, epsilon)))
#endif // __GFX__
static void calculate_body (int i, int body_count,
                            FTYPE * mass,
                            FTYPE * init_pos_x, FTYPE * init_pos_y,
                            FTYPE * init_pos_z, FTYPE * init_vec_x,
                            FTYPE * init_vec_y, FTYPE * init_vec_z,
                            FTYPE * test_pos_x, FTYPE * test_pos_y,
                            FTYPE * test_pos_z, FTYPE * test_vec_x,
                            FTYPE * test_vec_y, FTYPE * test_vec_z,
                            FTYPE time_step, FTYPE epsilon)
{
    FTYPE rx = init_pos_x[i], ry = init_pos_y[i], rz = init_pos_z[i];

    FTYPE ax = (FTYPE)0.0, ay = (FTYPE)0.0, az = (FTYPE)0.0;

    #pragma novector // improves performance on GT
    for (int j = 0; j < body_count; ++j) {
        FTYPE dx = init_pos_x[j] - rx,
        dy = init_pos_y[j] - ry,
        dz = init_pos_z[j] - rz,
        distanceInv = 1.0f/sqrtf(dx*dx + dy*dy + dz*dz + epsilon),
        K = mass[j]*distanceInv*distanceInv*distanceInv;

        ax += K*dx;
        ay += K*dy;
        az += K*dz;
    }

    FTYPE vx = init_vec_x[i],
          vy = init_vec_y[i],
          vz = init_vec_z[i];

    test_vec_x[i] = vx + ax*time_step;
    test_vec_y[i] = vy + ay*time_step;
    test_vec_z[i] = vz + az*time_step;

    test_pos_x[i] = rx + vx*time_step + ax*time_step*time_step/2.0f;
    test_pos_y[i] = ry + vy*time_step + ay*time_step*time_step/2.0f;
    test_pos_z[i] = rz + vz*time_step + az*time_step*time_step/2.0f;
}

template <class FTYPE>
void NBody<FTYPE>::execute_part(int do_offload, int off, int n)
{
    FTYPE time_step = this->time_step,
          epsilon   = this->epsilon;

    int body_count = init->N;

    FTYPE * mass = *masses;

    FTYPE * init_pos_x = init->rx, * init_pos_y = init->ry, * init_pos_z =
        init->rz;
    FTYPE * init_vec_x = init->vx, * init_vec_y = init->vy, * init_vec_z =
        init->vz;

    FTYPE * test_pos_x = out->rx, * test_pos_y = out->ry, * test_pos_z =
        out->rz;
    FTYPE * test_vec_x = out->vx, * test_vec_y = out->vy, * test_vec_z =
        out->vz;

#pragma offload target(gfx) if (do_offload > 0)\
    pin(mass, init_pos_x, init_pos_y, init_pos_z, init_vec_x, init_vec_y,\
        init_vec_z, test_pos_x, test_pos_y, test_pos_z, test_vec_x,\
        test_vec_y, test_vec_z: length(body_count))
    _Cilk_for _Simd (int i = off; i < off + n; ++i) {
        calculate_body (i, body_count, mass,
                        init_pos_x, init_pos_y, init_pos_z,
                        init_vec_x, init_vec_y, init_vec_z,
                        test_pos_x, test_pos_y, test_pos_z,
                        test_vec_x, test_vec_y, test_vec_z,
                        time_step, epsilon);
    }
}

template <class FTYPE>
void NBodyLocals<FTYPE>::execute_part(int do_offload, int off, int n)
{
    FTYPE time_step = this->time_step;
    FTYPE epsilon = this->epsilon;

    FTYPE *mass = *this->masses;

    FTYPE *init_pos_x = this->init->rx;
    FTYPE *init_pos_y = this->init->ry;
    FTYPE *init_pos_z = this->init->rz;
    FTYPE *init_vec_x = this->init->vx;
    FTYPE *init_vec_y = this->init->vy;
    FTYPE *init_vec_z = this->init->vz;

    FTYPE *test_pos_x = this->out->rx;
    FTYPE *test_pos_y = this->out->ry;
    FTYPE *test_pos_z = this->out->rz;
    FTYPE *test_vec_x = this->out->vx;
    FTYPE *test_vec_y = this->out->vy;
    FTYPE *test_vec_z = this->out->vz;

    int body_count = this->init->N;

    #pragma offload target(gfx) if (do_offload > 0)\
        pin(mass, init_pos_x, init_pos_y, init_pos_z, init_vec_x, init_vec_y,\
            init_vec_z, test_pos_x, test_pos_y, test_pos_z, test_vec_x,\
            test_vec_y, test_vec_z: length(body_count))
    _Cilk_for (int i = off; i < off + n; i+=TILE_i) {
        FTYPE rx[TILE_i], ry[TILE_i], rz[TILE_i];
        FTYPE ax[TILE_i], ay[TILE_i], az[TILE_i];
        FTYPE ix[TILE_j], iy[TILE_j], iz[TILE_j];
        FTYPE mm[TILE_j];

        #pragma simd
        for (int k = 0; k<TILE_i; k++) {
            rx[k] = init_pos_x[i+k];
            ry[k] = init_pos_y[i+k];
            rz[k] = init_pos_z[i+k];
            ax[k] = 0;
            ay[k] = 0;
            az[k] = 0;
        }

        for (int j = 0; j < body_count; j += TILE_j) {
            #pragma simd
            for (int j_ = 0; j_ < TILE_j; j_++) {
                ix[j_] = init_pos_x[j + j_];
                iy[j_] = init_pos_y[j + j_];
                iz[j_] = init_pos_z[j + j_];
                mm[j_] = mass[j + j_];
            }

            #pragma unroll
            for (int j_ = 0; j_ < TILE_j; j_++) {
                #pragma simd
                for (int k=0; k<TILE_i; k++) {
                    FTYPE dx = ix[j_] - rx[k],
                        dy = iy[j_] - ry[k],
                        dz = iz[j_] - rz[k],
                        distanceInv = 1.0f/sqrtf(dx*dx + dy*dy + dz*dz +
                            epsilon),
                        K = mm[j_]*distanceInv*distanceInv*distanceInv;

                    ax[k] += K*dx;
                    ay[k] += K*dy;
                    az[k] += K*dz;
                }
            }
        }
        #pragma simd
        for (int k = 0; k<TILE_i; k++) {
            FTYPE vx = init_vec_x[i+k];
            FTYPE vy = init_vec_y[i+k];
            FTYPE vz = init_vec_z[i+k];
            test_vec_x[i+k] = vx + ax[k]*time_step;
            test_vec_y[i+k] = vy + ay[k]*time_step;
            test_vec_z[i+k] = vz + az[k]*time_step;
            test_pos_x[i+k] = rx[k] + vx*time_step +
                ax[k]*time_step*time_step/2.0f;
            test_pos_y[i+k] = ry[k] + vy*time_step +
                ay[k]*time_step*time_step/2.0f;
            test_pos_z[i+k] = rz[k] + vz*time_step +
                az[k]*time_step*time_step/2.0f;
        }
    }
}

template <class FTYPE>
void NBodyLocals_AOS<FTYPE>::execute_part(int do_offload, int off, int n)
{
    FTYPE time_step = this->time_step;
    FTYPE epsilon = this->epsilon;

    int body_count = this->init->N;

    FTYPE4 * init_pos4 = this->init_pos4, * init_vel4 = this->init_vel4,
           * out_pos4 = this->out_pos4, * out_vel4 = this->out_vel4;

    #pragma offload target(gfx) if (do_offload > 0)\
        pin(init_pos4, init_vel4, out_pos4, out_vel4: length(body_count))
    _Cilk_for (int i = off; i < off + n; i += TILE_i) {
        FTYPE rx[TILE_i], ry[TILE_i], rz[TILE_i];
        FTYPE ax[TILE_i], ay[TILE_i], az[TILE_i];
        FTYPE ix[TILE_j], iy[TILE_j], iz[TILE_j];
        FTYPE mm[TILE_j];

        AOS2SOA(rx, ry, rz, init_pos4 + i, TILE_i);

        ax[:] = 0; ay[:] = 0; az[:] = 0;

        for (int j = 0; j < body_count; j += TILE_j) {
            AOS2SOA(ix, iy, iz, mm, init_pos4 + j, TILE_j);

            #pragma unroll
            for (int j_ = 0; j_ < TILE_j; j_++) {
                #pragma simd
                for (int k=0; k<TILE_i; k++) {
                    FTYPE dx = ix[j_] - rx[k],
                          dy = iy[j_] - ry[k],
                          dz = iz[j_] - rz[k];
                    FTYPE distanceInv = 1.0f / sqrtf(dx*dx + dy*dy + dz*dz +
                        epsilon);
                    FTYPE K = mm[j_] * distanceInv * distanceInv * distanceInv;

                    ax[k] += K * dx;
                    ay[k] += K * dy;
                    az[k] += K * dz;
                }
            }
        }

        #pragma simd
        for (int k = 0; k<TILE_i; k++) {
            FTYPE vx = init_vel4[i+k].x;
            FTYPE vy = init_vel4[i+k].y;
            FTYPE vz = init_vel4[i+k].z;

            out_vel4[i+k].x = vx + ax[k] * time_step;
            out_vel4[i+k].y = vy + ay[k] * time_step;
            out_vel4[i+k].z = vz + az[k] * time_step;

            out_pos4[i+k].x = rx[k] + vx * time_step + ax[k] * time_step *
                time_step * 0.5f;
            out_pos4[i+k].y = ry[k] + vy * time_step + ay[k] * time_step *
                time_step * 0.5f;
            out_pos4[i+k].z = rz[k] + vz * time_step + az[k] * time_step *
                time_step * 0.5f;
        }
    }
}
