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

#include "fluid_animateAN.h"
#include "fluid_animate.h"
#include "cell_poolAN.h"
#include "IO.h"

#include <cassert>
#include <cstring>
#include <xmmintrin.h>
#include <algorithm>

// pool to allocate cells to hold overflow particles (where # of particles > c_particles_per_cell)
cellpoolAN poolAN;

// total number of CellAN cells
int numCellsAN;
// number of CellAN cells in each direction
int numCellsAN_x, numCellsAN_y, numCellsAN_z;
// Dimensions of a CellAN cell -- must be wider than nearest neighbor radius hAN
Vec3AN deltaAN;
// primary cell structure
CellAN *cellsAN;
// secondary cell structure used as temporary store on buffer exchange
CellAN *cells2AN;
// number of particles in each cell
int *cell_num_particlesAN;
int *cell_num_particles2AN;
// keep track of leftover cells during buffer exchange
CellAN **last_cellsAN;
// radius of nearest neighbor sphere
float hAN, hSqAN;
// constants used to describe the fluid
float densityCoeffAN, pressureCoeffAN, viscosityCoeffAN;

Vec3AN vMaxAN(0.f,0.f,0.f);
Vec3AN vMinAN(0.f,0.f,0.f);

using namespace std;

// given a position pos, find in which cell the particle falls
//
// [in]: pos
// [out]: index of cell into cellsAN
int get_cell_index(Vec3AN pos) {
	int cell_off[3];
	cell_off[:] = (int)((pos.vec[0:3] - c_domain_minAN.vec[0:3]) / deltaAN.vec[0:3]);

	if(cell_off[0] < 0) cell_off[0] = 0; else if(cell_off[0] > (numCellsAN_x-1)) cell_off[0] = numCellsAN_x-1;
	if(cell_off[1] < 0) cell_off[1] = 0; else if(cell_off[1] > (numCellsAN_y-1)) cell_off[1] = numCellsAN_y-1;
	if(cell_off[2] < 0) cell_off[2] = 0; else if(cell_off[2] > (numCellsAN_z-1)) cell_off[2] = numCellsAN_z-1;
	
	return (cell_off[2]*numCellsAN_y + cell_off[1])*numCellsAN_x + cell_off[0];
}

// Initializes information about simulation and cells (containing particles)
// differs from init_sim due to array notation Vec3AN
void init_simAN(string filename) {
	open_read_file(filename);
	float restParticlesPerMeter;
	int num_particles;
	RPPM_and_numPart ran = get_RPPM_and_numPart();
	restParticlesPerMeter = ran.restParticlesPerMeter;
	num_particles = ran.numParticles;
	hAN = c_kernel_radius_multiplier / restParticlesPerMeter;
	hSqAN = hAN*hAN;
	poolAN.initialize(num_particles);

	Vec3AN range = c_domain_maxAN - c_domain_minAN;
	numCellsAN_x = (int)(range.vec[0] / hAN);
	numCellsAN_y = (int)(range.vec[1] / hAN);
	numCellsAN_z = (int)(range.vec[2] / hAN);
	assert(numCellsAN_x >= 1 && numCellsAN_y >= 1 && numCellsAN_z >= 1);
	numCellsAN = numCellsAN_x*numCellsAN_y*numCellsAN_z;

	deltaAN.vec[0] = range.vec[0] / numCellsAN_x;
	deltaAN.vec[1] = range.vec[1] / numCellsAN_y;
	deltaAN.vec[2] = range.vec[2] / numCellsAN_z;
	assert(deltaAN.vec[0] >= hAN && deltaAN.vec[1] >= hAN && deltaAN.vec[2] >= hAN);
	
	assert(sizeof(CellAN) % c_cacheline_size == 0);
	assert(offsetof(Cell_auxAN, padding) == offsetof(CellAN, padding));

	cellsAN = (CellAN *)_mm_malloc(sizeof(CellAN)*numCellsAN, c_cacheline_size);
	cells2AN = (CellAN *)_mm_malloc(sizeof(CellAN)*numCellsAN, c_cacheline_size);
	cell_num_particlesAN = (int *)_mm_malloc(sizeof(int)*numCellsAN, c_cacheline_size);
	cell_num_particles2AN = (int *)_mm_malloc(sizeof(int)*numCellsAN, c_cacheline_size);
	last_cellsAN = (CellAN **)_mm_malloc(sizeof(CellAN *) * numCellsAN, c_cacheline_size);
	
	assert(cellsAN && cells2AN && cell_num_particlesAN && cell_num_particles2AN);

	for(int i=0; i<numCellsAN; ++i) {
		new (cellsAN+i) CellAN;
		new (cells2AN+i) CellAN;
	}

	memset(cell_num_particlesAN, 0, numCellsAN*sizeof(int));

	for(int i = 0; i < num_particles; ++i) {
		pardata pd = get_particle();
		Vec3AN p,hv,v;
		p = Vec3AN(pd.px, pd.py, pd.pz);
		hv = Vec3AN(pd.hvx, pd.hvy, pd.hvz);
		v = Vec3AN(pd.vx, pd.vy, pd.vz);

		int index = get_cell_index(p);
		CellAN *cell = cellsAN+index;

		int num_part = cell_num_particlesAN[index];
		// go to last overflow cell in list
		// cell.next will always be valid due to "if" statement underneath
		while(num_part > c_particles_per_cell) {
			cell = cell->next;
			num_part -= c_particles_per_cell;
		}
		// if cell is full, get a cell from the overflow poolAN
		if((num_part % c_particles_per_cell == 0) && (cell_num_particlesAN[index] != 0)) {
			cell->next = poolAN.get_cell();
			cell = cell->next;
			num_part -= c_particles_per_cell;
		}
		// add particle to cell
		cell->p[num_part] = p;
		cell->hv[num_part] = hv;
		cell->v[num_part] = v;
		
		vMinAN.vec[0] = min(vMinAN.vec[0], cell->v[num_part].vec[0]);
		vMaxAN.vec[0] = max(vMaxAN.vec[0], cell->v[num_part].vec[0]);
		vMinAN.vec[1] = min(vMinAN.vec[1], cell->v[num_part].vec[1]);
		vMaxAN.vec[1] = max(vMaxAN.vec[1], cell->v[num_part].vec[1]);
		vMinAN.vec[2] = min(vMinAN.vec[2], cell->v[num_part].vec[2]);
		vMaxAN.vec[2] = max(vMaxAN.vec[2], cell->v[num_part].vec[2]);
		

		++cell_num_particlesAN[index];
	}
	
	close_read_file();
	
	float pi = static_cast<float>(M_PI);
	float coeff1 = 315.f / (64.f*pi*pow(hAN,9.f));
	float coeff2 = 15.f / (pi*pow(hAN,6.f));
	float coeff3 = 45.f / (pi*pow(hAN,6.f));
	float particleMass = 0.5f*c_double_rest_density / (restParticlesPerMeter*restParticlesPerMeter*restParticlesPerMeter);
	densityCoeffAN = particleMass * coeff1;
	pressureCoeffAN = 3.f*coeff2 * 0.5f*c_stiffness_pressure * particleMass;
	viscosityCoeffAN = c_viscosity * coeff3 * particleMass;
}

// exchanges the buffer cells by swapping cells and cells2
// recalculates which particles go into which cells
//
// [in]: cells2AN, cell_num_particles2AN
// [out]: cellsAN, cell_num_particlesAN
void rebuild_gridAN() {
	// swap src and dest arrays (almost like switching screen buffers)
	swap(cellsAN, cells2AN);
	swap(cell_num_particlesAN, cell_num_particles2AN);

	// Initialize dest
	memset(cell_num_particlesAN, 0, numCellsAN*sizeof(int));
	for(int i=0; i<numCellsAN; ++i) {
		cellsAN[i].next = NULL;
		last_cellsAN[i] = cellsAN+i;
	}

	// iterate through src cells
	for(int i = 0; i < numCellsAN; ++i) {
		CellAN *cell2 = cells2AN+i;
		for(int j = 0; j < cell_num_particles2AN[i]; ++j) {
			// get last pointer in correct dest cells
			int index = get_cell_index(cell2->p[j%c_particles_per_cell]);
			CellAN *cell = last_cellsAN[index];
			int part = cell_num_particlesAN[index];

			// grab an overflow cell if necessary
			if((part % c_particles_per_cell == 0) && (cell_num_particlesAN[index] != 0)) {
				cell->next = poolAN.get_cell();
				cell = cell->next;
				last_cellsAN[index] = cell;
			}
			++cell_num_particlesAN[index];

			// copy src to dest particle
			cell->p[part%c_particles_per_cell] = cell2->p[j%c_particles_per_cell];
			cell->hv[part%c_particles_per_cell] = cell2->hv[j%c_particles_per_cell];
			cell->v[part%c_particles_per_cell] = cell2->v[j%c_particles_per_cell];

			// move to next overflow src cell if necessary
			if(j%c_particles_per_cell == c_particles_per_cell-1) {
				CellAN *temp = cell2;
				cell2 = cell2->next;
				// return overflow cells to poolAN (and not statically allocated cells)
				if(temp != cells2AN+i) {
					poolAN.return_cell(temp);
				}
			}
		}
		// if cell2 is overflow, return to poolAN
		if(cell2 && (cell2 != cells2AN+i)) {
			poolAN.return_cell(cell2);
		}
	}
}

// gets only half of the surrounding neighbor cells, given a certain x,y,z coordinate cell
// This is required so that each combination of particles is only considered once
// fills neigh_cells with indexes of neighbor cells
// returns number of neighbors
int get_neighbor_cellsAN(int cell_x, int cell_y, int cell_z, int *neigh_cells) {
	int num_neigh_cells = 0;

	// get self first
	neigh_cells[num_neigh_cells++] = (cell_z*numCellsAN_y + cell_y) * numCellsAN_x + cell_x;
		
	// find surrounding neighbors
	for(int dx = -1; dx <= 1; ++dx)
		for(int dy = -1; dy <= 1; ++dy)
			for(int dz = -1; dz <= 1; ++dz) {
				int xx = cell_x + dx;
				int yy = cell_y + dy;
				int zz = cell_z + dz;
				if(xx >= 0 && xx < numCellsAN_x && yy >= 0 && yy < numCellsAN_y && zz >= 0 && zz < numCellsAN_z) {
					int index = (zz*numCellsAN_y + yy)*numCellsAN_x + xx;
					// Only add cells lower than current cell so that calculations are only done once per pair
					// Also make sure there are actually particles to do work on
					if((index < neigh_cells[0]) && (cell_num_particlesAN[index] != 0)) {
						neigh_cells[num_neigh_cells++] = index;
					}
				}
			}

	return num_neigh_cells;
}

// Computes the density of each particle based on number of nearby neighbors
// 
// [in]: cell.p, neigh.p, hSq
// [out]: cell.density, neigh.density
void compute_densitiesAN() {
	int cell_index = 0;
	
	for(int cell_z = 0; cell_z < numCellsAN_z; ++cell_z) {
		for(int cell_y = 0; cell_y < numCellsAN_y; ++cell_y) {
			for(int cell_x = 0; cell_x < numCellsAN_x; ++cell_x, ++cell_index) {
				int num_pars = cell_num_particlesAN[cell_index];
				if(num_pars == 0)
					continue;

				int neigh_cells[27];
				int num_neigh_cells = get_neighbor_cellsAN(cell_x, cell_y, cell_z, neigh_cells);

				CellAN *cell = cellsAN+cell_index;
				// For each particle in the cell
				for(int par_index = 0; par_index < num_pars; ++par_index) {
					int ipar = par_index%c_particles_per_cell;
					// The first index in neigh_cells is always cell_index
					// Thus, cell = neigh. This requires special treatment
					int neigh_cell_index = neigh_cells[0];
					CellAN *neigh = cellsAN+neigh_cell_index;
					// only iterate to par_index to make sure you do not consider a combination twice
					for(int neigh_par_index = 0; neigh_par_index < par_index; ++neigh_par_index) {
						int ineigh_par = neigh_par_index%c_particles_per_cell;
						// Actual kernel calculation for density occurs here
						float distSq = (cell->p[ipar] - neigh->p[ineigh_par]).GetLengthSq();
						if(distSq < hSqAN) {
							float t = hSqAN - distSq;
							float tc = t*t*t;
							cell->density[ipar] += tc;
							neigh->density[ineigh_par] += tc;
						}
						// End kernel
						//move pointer to next cell in list if end of array is reached
						if(ineigh_par == c_particles_per_cell-1) {
							neigh = neigh->next;
						}
					}
					
					// Now the remainder of the cells have no limitations. Do work on every particle in every cell
					// For each neighbor cell
					for(int inc = 1; inc < num_neigh_cells; ++inc) {
						neigh_cell_index = neigh_cells[inc];
						neigh = cellsAN+neigh_cell_index;
						// For each particle in the neighbor cell
						for(int neigh_par_index = 0; neigh_par_index < cell_num_particlesAN[neigh_cell_index]; ++neigh_par_index) {
							int ineigh_par = neigh_par_index%c_particles_per_cell;
							// Actual kernel calculation for density occurs here
							float distSq = (cell->p[ipar] - neigh->p[ineigh_par]).GetLengthSq();
							if(distSq < hSqAN) {
								float t = hSqAN - distSq;
								float tc = t*t*t;
								cell->density[ipar] += tc;
								neigh->density[ineigh_par] += tc;
							}
							// End kernel
							//move pointer to next cell in list if end of array is reached
							if(ineigh_par == c_particles_per_cell-1) {
								neigh = neigh->next;
							}
						}
					}
					//move pointer to next cell in list if end of array is reached
					if(ipar == c_particles_per_cell-1) {
						cell = cell->next;
					}
				}
			}
		}
	}

	const float tc = hSqAN*hSqAN*hSqAN;
	for(int i = 0; i < numCellsAN; ++i) {
		CellAN *cell = cellsAN+i;
		for(int j = 0; j < cell_num_particlesAN[i]; ++j) {
			cell->density[j%c_particles_per_cell] += tc;
			cell->density[j%c_particles_per_cell] *= densityCoeffAN;
			if(j % c_particles_per_cell == c_particles_per_cell-1) {
				cell = cell->next;
			}
		}
	}
}

// Computes the acceleration on each particle based on number of nearest neighbors
// and previously calculated densities
//
// [in]: cell.p, neigh.p, h, pressureCoeff, cell.density, neigh.density, c_double_rest_density
//       cell.v, neigh.v, c_viscosityCoeff
// [out]: cell.a, neigh.a
void compute_accelerationsAN() {
	int cell_index = 0;
	
	for(int cell_z = 0; cell_z < numCellsAN_z; ++cell_z) {
		for(int cell_y = 0; cell_y < numCellsAN_y; ++cell_y) {
			for(int cell_x = 0; cell_x < numCellsAN_x; ++cell_x, ++cell_index) {
				int num_pars = cell_num_particlesAN[cell_index];
				if(num_pars == 0)
					continue;

				int neigh_cells[27];
				int num_neigh_cells = get_neighbor_cellsAN(cell_x, cell_y, cell_z, neigh_cells);

				CellAN *cell = cellsAN+cell_index;
				// For each particle in the cell
				for(int par_index = 0; par_index < num_pars; ++par_index) {
					int ipar = par_index%c_particles_per_cell;
					// The first index in neigh_cells is always cell_index
					// Thus, cell = neigh. This requires special treatment
					int neigh_cell_index = neigh_cells[0];
					CellAN *neigh = cellsAN+neigh_cell_index;
					// only iterate to par_index to make sure you do not consider a combination twice
					for(int neigh_par_index = 0; neigh_par_index < par_index; ++neigh_par_index) {
						int ineigh_par = neigh_par_index%c_particles_per_cell;
						// Actual kernel calculation for acceleration occurs here
						Vec3AN disp = cell->p[ipar] - neigh->p[ineigh_par];
						float distSq = disp.GetLengthSq();
						if(distSq < hSqAN) {
							float dist = sqrtf(max(distSq, 1e-12f));

							float hmr = hAN - dist;

							Vec3AN acc = disp * pressureCoeffAN * (hmr*hmr/dist) * (cell->density[ipar]+neigh->density[ineigh_par] - c_double_rest_density);
							acc += (neigh->v[ineigh_par] - cell->v[ipar]) * viscosityCoeffAN * hmr;
							acc /= cell->density[ipar] * neigh->density[ineigh_par];

							cell->a[ipar] += acc;
							neigh->a[ineigh_par] -= acc;
						}
						// End kernel
						//move pointer to next cell in list if end of array is reached
						if(ineigh_par == c_particles_per_cell-1) {
							neigh = neigh->next;
						}
					}
					// Now the remainder of the cells have no limitations. Do work on every particle in every cell
					// For each neighbor cell
					for(int inc = 1; inc < num_neigh_cells; ++inc) {
						neigh_cell_index = neigh_cells[inc];
						neigh = cellsAN+neigh_cell_index;
						// For each particle in the neighbor cell
						for(int neigh_par_index = 0; neigh_par_index < cell_num_particlesAN[neigh_cell_index]; ++neigh_par_index) {
							int ineigh_par = neigh_par_index%c_particles_per_cell;
							// Actual kernel calculation for acceleration occurs here
							Vec3AN disp = cell->p[ipar] - neigh->p[ineigh_par];
							float distSq = disp.GetLengthSq();
							if(distSq < hSqAN) {
								float dist = sqrtf(max(distSq, 1e-12f));

								float hmr = hAN - dist;

								Vec3AN acc = disp * pressureCoeffAN * (hmr*hmr/dist) * (cell->density[ipar]+neigh->density[ineigh_par] - c_double_rest_density);
								acc += (neigh->v[ineigh_par] - cell->v[ipar]) * viscosityCoeffAN * hmr;
								acc /= cell->density[ipar] * neigh->density[ineigh_par];

								cell->a[ipar] += acc;
								neigh->a[ineigh_par] -= acc;
							}
							// End kernel
							//move pointer to next cell in list if end of array is reached
							if(ineigh_par == c_particles_per_cell-1) {
								neigh = neigh->next;
							}
						}
					}
					//move pointer to next cell in list if end of array is reached
					if(ipar == c_particles_per_cell-1) {
						cell = cell->next;
					}
				}
			}
		}
	}
}

// Resets density and acceleration, then recomputes
void compute_forcesAN() {
	for(int i = 0; i < numCellsAN; ++i) {
		CellAN *cell = cellsAN+i;
		for(int j = 0; j < cell_num_particlesAN[i]; ++j) {
			cell->density[j%c_particles_per_cell] = 0.f;
			cell->a[j%c_particles_per_cell] = c_external_accelerationAN;
			// move pointer to next overflow cell if end of current cell reached
			if(j % c_particles_per_cell == c_particles_per_cell-1) {
				cell=cell->next;
			}
		}
	}

	compute_densitiesAN();
	compute_accelerationsAN();
}

// general function to check if particle escapes from a minimum wall and prevent it
// works for either min x, y, or z
//
// [in]: x,y,z cell coordinate, index (either 0,1,or 2 for x,y,z)
// [out]: updated acceleration
void check_if_past_min_wallAN(int x, int y, int z, int index) {
	int cwall = (z*numCellsAN_y + y)*numCellsAN_x + x;
	CellAN *cell = cellsAN+cwall;
	for(int i=0; i<cell_num_particlesAN[cwall]; ++i) {
		int i_index = i % c_particles_per_cell;
		float cell_p = cell->p[i_index].vec[index];
		float cell_hv = cell->hv[i_index].vec[index];
		float pos_x = cell_p + cell_hv * c_time_step;

		const float idomain_minAN = c_domain_minAN.vec[index];
		float diff = c_par_size - (pos_x - idomain_minAN);
		if(diff > c_epsilon) {
			float &cell_a = cell->a[i_index].vec[index];
			float cell_v = cell->v[i_index].vec[index];
			cell_a += c_stiffness_collisions*diff - c_damping*cell_v;
		}
		// if cell reaches end of cell, move to next overflow cell
		if(i % c_particles_per_cell == c_particles_per_cell - 1) {
			cell = cell->next;
		}
	}
}

// general function to check if particle escapes from a maximum wall and prevent it
// works for either max x, y, or z
//
// [in]: x,y,z cell coordinate, index (either 0,1,or 2 for x,y,z)
// [out]: updated acceleration
void check_if_past_max_wallAN(int x, int y, int z, int index) {
	int cwall = (z*numCellsAN_y + y)*numCellsAN_x + x;
	CellAN *cell = cellsAN+cwall;
	for(int i=0; i<cell_num_particlesAN[cwall]; ++i) {
		int i_index = i % c_particles_per_cell;
		float cell_p = cell->p[i_index].vec[index];
		float cell_hv = cell->hv[i_index].vec[index];
		float pos_x = cell_p + cell_hv * c_time_step;

		const float idomain_maxAN = c_domain_maxAN.vec[index];
		float diff = c_par_size - (idomain_maxAN - pos_x);
		if(diff > c_epsilon) {
			float &cell_a = cell->a[i_index].vec[index];
			float cell_v = cell->v[i_index].vec[index];
			cell_a += c_stiffness_collisions*diff - c_damping*cell_v;
		}
		// if cell reaches end of cell, move to next overflow cell
		if(i % c_particles_per_cell == c_particles_per_cell - 1) {
			cell = cell->next;
		}
	}
}

// Loops for checking if particle escapes from a wall and dampens the acceleration to prevent it
// for x walls, pass 0 index to min and max wall check
// for y walls, pass 1 index to min and max wall check
// for z walls, pass 2 index to min and max wall check
void process_collisionsAN() {
	int x,y,z;
	x=0;  // along the c_domain_minAN.x wall
	int xmax=numCellsAN_x-1;  // along the c_domain_maxAN.x wall
	for(y=0; y<numCellsAN_y; ++y) {
		for(z=0; z<numCellsAN_z; ++z) {
			check_if_past_min_wallAN(x,y,z,0);
			check_if_past_max_wallAN(xmax,y,z,0);
		}
	}

	y=0;  // along the c_domain_minAN.y wall
	int ymax=numCellsAN_y-1;  // along the c_domain_maxAN.y wall
	for(x=0; x<numCellsAN_x; ++x) {
		for(z=0; z<numCellsAN_z; ++z) {
			check_if_past_min_wallAN(x,y,z,1);
			check_if_past_max_wallAN(x,ymax,z,1);
		}
	}

	z=0;  // along the c_domain_minAN.z wall
	int zmax=numCellsAN_z-1;  // along the c_domain_maxAN.z wall
	for(x=0; x<numCellsAN_x; ++x) {
		for(y=0; y<numCellsAN_y; ++y) {
			check_if_past_min_wallAN(x,y,z,2);
			check_if_past_max_wallAN(x,y,zmax,2);
		}
	}
}

// second pass over the minimum edges of the domain
// some particles could be moved out of bounds and then printed to the screen otherwise
// To be called after advance_particles
//
// [in]: x,y,z cell coordinate, index (either 0,1,or 2 for x,y,z)
// [out]: updated p, v, and hv
void check_if_past_min_wall2AN(int x, int y, int z, int index) {
	int cwall = (z*numCellsAN_y + y)*numCellsAN_x + x;
	CellAN *cell = cellsAN+cwall;
	for(int i=0; i<cell_num_particlesAN[cwall]; ++i) {
		int i_index = i % c_particles_per_cell;
		float &cell_p = cell->p[i_index].vec[index];
		float idomain_minAN = c_domain_minAN.vec[index];
		float diff = cell_p - idomain_minAN;
		if(diff < 0.f) {
			float &cell_v = cell->v[i_index].vec[index];
			float &cell_hv = cell->hv[i_index].vec[index];
			cell_p = idomain_minAN - diff;
			cell_v = -cell_v;
			cell_hv = -cell_hv;
		}
		// if cell reaches end of cell, move to next overflow cell
		if(i % c_particles_per_cell == c_particles_per_cell - 1) {
			cell = cell->next;
		}
	}
}

// second pass over the maximum edges of the domain
// some particles could be moved out of bounds and then printed to the screen otherwise
// To be called after advance_particles
//
// [in]: x,y,z cell coordinate, index (either 0,1,or 2 for x,y,z)
// [out]: updated p, v, and hv
void check_if_past_max_wall2AN(int x, int y, int z, int index) {
	int cwall = (z*numCellsAN_y + y)*numCellsAN_x + x;
	CellAN *cell = cellsAN+cwall;
	for(int i=0; i<cell_num_particlesAN[cwall]; ++i) {
		int i_index = i % c_particles_per_cell;
		float &cell_p = cell->p[i_index].vec[index];
		const float idomain_max = c_domain_maxAN.vec[index];
		float diff = idomain_max - cell_p;
		if(diff < 0.f) {
			float &cell_v = cell->v[i_index].vec[index];
			float &cell_hv = cell->hv[i_index].vec[index];
			cell_p = idomain_max + diff;
			cell_v = -cell_v;
			cell_hv = -cell_hv;
		}
		// if cell reaches end of cell, move to next overflow cell
		if(i % c_particles_per_cell == c_particles_per_cell - 1) {
			cell = cell->next;
		}
	}
}

// This function makes a second pass over the edges of the domain
// some particles could be moved out of bounds and then printed to the screen otherwise
// To be called after advance_particles
// for x walls, pass 0 index to min and max wall check
// for y walls, pass 1 index to min and max wall check
// for z walls, pass 2 index to min and max wall check
void process_collisions2AN() {
	int x,y,z;
	x=0;// along the c_domain_min.x wall
	int xmax = numCellsAN_x-1;  // along the c_domain_max.x wall
	for(y=0; y<numCellsAN_y; ++y) {
		for(z=0; z<numCellsAN_z; ++z) {
			check_if_past_min_wall2AN(x,y,z,0);
			check_if_past_max_wall2AN(xmax,y,z,0);
		}
	}

	y=0;  // along the c_domain_min.y wall
	int ymax=numCellsAN_y-1;  // along the c_domain_max.y wall
	for(x=0; x<numCellsAN_x; ++x) {
		for(z=0; z<numCellsAN_z; ++z) {
			check_if_past_min_wall2AN(x,y,z,1);
			check_if_past_max_wall2AN(x,ymax,z,1);
		}
	}

	z=0;  // along the c_domain_min.z wall
	int zmax=numCellsAN_z-1;  // along the c_domain_max.z wall
	for(x=0; x<numCellsAN_x; ++x) {
		for(y=0; y<numCellsAN_y; ++y) {
			check_if_past_min_wall2AN(x,y,z,2);
			check_if_past_max_wall2AN(x,y,zmax,2);
		}
	}
}

// advances particle positions based on their hv (half velocity)
// updates velocity based on calculated acceleration
// [in]: cell.hv, cell.a
// [out]: cell.p, cell.v, cell.hv
void advance_particlesAN() {
	for(int i = 0; i < numCellsAN; ++i) {
		CellAN *cell = cellsAN+i;
		for(int j = 0; j < cell_num_particlesAN[i]; ++j) {
			Vec3AN v_half = cell->hv[j % c_particles_per_cell] + cell->a[j % c_particles_per_cell]*c_time_step;
			cell->p[j%c_particles_per_cell] += v_half * c_time_step;
			cell->v[j%c_particles_per_cell] = (cell->hv[j%c_particles_per_cell] + v_half) * 0.5f;
			cell->hv[j%c_particles_per_cell] = v_half;

			// if cell reaches end of cell, move to next overflow cell
			if(j % c_particles_per_cell == c_particles_per_cell - 1) {
				cell = cell->next;
			}
		}
	}
}

// dispatching function
// Each completion of this function represents a time step
// Just calls the functions with AN
void advance_frameAN() {
	rebuild_gridAN();
	compute_forcesAN();
	// updates acceleration
	process_collisionsAN();
	advance_particlesAN();
	// fixes any particle position that moved across domain
	process_collisions2AN();
}

// Delete all cells and cellpool
void clean_up_simAN() {
	// first return all overflow cells to the poolAN
	for(int i=0; i<numCellsAN; ++i) {
		CellAN& cell = cellsAN[i];
		while(cell.next) {
			CellAN *temp = cell.next;
			cell.next = temp->next;
			poolAN.return_cell(temp);
		}
	}
	// normally you could just reuse cellpoolAN, but to keep memory work consistent across runs, explicitly free the poolAN
	poolAN.~cellpoolAN();
	_mm_free(cellsAN);
	_mm_free(cells2AN);
	_mm_free(cell_num_particlesAN);
	_mm_free(cell_num_particles2AN);
	_mm_free(last_cellsAN);
}

// Save particles to file if necessary, cleans up allocated cell information
void end_simAN(string fileName) {
	if(!fileName.empty()) {
		open_save_file(fileName);
		save_RPPM_and_numPart();
		for(int i = 0; i < numCellsAN; ++i) {
			CellAN *cell = cellsAN+i;
			for(int j = 0; j < cell_num_particlesAN[i]; ++j) {
				save_particle(cell->p[j % c_particles_per_cell].vec[0], cell->p[j % c_particles_per_cell].vec[1], cell->p[j % c_particles_per_cell].vec[2],
					cell->hv[j % c_particles_per_cell].vec[0], cell->hv[j % c_particles_per_cell].vec[1], cell->hv[j % c_particles_per_cell].vec[2],
					cell->v[j % c_particles_per_cell].vec[0], cell->v[j % c_particles_per_cell].vec[1], cell->v[j % c_particles_per_cell].vec[2]);

				// move to next overflow cell if end of the current cell
				if(j % c_particles_per_cell == c_particles_per_cell -1) {
					cell = cell->next;
				}
			}
		}
		close_save_file();
	}
	clean_up_simAN();
}
