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
#include "IO.h"
#include "cell_pool.h"

#include <xmmintrin.h>
#include <cstring>
#include <cassert>
#include <algorithm>


// pool to allocate cells to hold overflow particles (where # of particles > c_particles_per_cell)
cellpool pool;

// Total number of Cell cells in simulation
int numCells;
// number of Cell cells in each direction
int numCells_x, numCells_y, numCells_z;
// Dimensions of a Cell cell -- must be wider than nearest neighbor radius h
Vec3 delta;
// primary cell structure
Cell *cells;
// secondary cell structure used as temporary store on buffer exchange
Cell *cells2;
// number of particles in each cell
int *cell_num_particles;
int *cell_num_particles2;
// keep track of leftover cells during buffer exchange
Cell **last_cells;
// radius of nearest neighbor sphere
float h, hSq;
// constants used to describe the fluid
float densityCoeff, pressureCoeff, viscosityCoeff;

// gets the surrounding neighbor cells, given a certain x,y,z coordinate cell
// fills neigh_cells with indexes of neighbor cells
// returns number of neighbors
int get_neighbor_cells(int cell_x, int cell_y, int cell_z, int *neigh_cells);

using namespace std;

// given a position pos, find in which cell the particle falls
//
// [in]: position of a particle, (px,py,pz)
// [out]: index of cell into cells
int get_cell_index(float px, float py, float pz) {
	int cell_x = (int)((px - c_domain_min.x) / delta.x);
	int cell_y = (int)((py - c_domain_min.y) / delta.y);
	int cell_z = (int)((pz - c_domain_min.z) / delta.z);

	if(cell_x < 0) cell_x = 0; else if(cell_x > (numCells_x-1)) cell_x = numCells_x-1;
	if(cell_y < 0) cell_y = 0; else if(cell_y > (numCells_y-1)) cell_y = numCells_y-1;
	if(cell_z < 0) cell_z = 0; else if(cell_z > (numCells_z-1)) cell_z = numCells_z-1;
	
	return (cell_z*numCells_y + cell_y)*numCells_x + cell_x;
}

// Initializes information about simulation and cells (containing particles)
void init_sim(string filename) {
	open_read_file(filename);
	float restParticlesPerMeter;
	int num_particles;
	RPPM_and_numPart ran = get_RPPM_and_numPart();
	restParticlesPerMeter = ran.restParticlesPerMeter;
	num_particles = ran.numParticles;
	h = c_kernel_radius_multiplier / restParticlesPerMeter;
	hSq = h*h;
	pool.initialize(num_particles);

	Vec3 range = c_domain_max - c_domain_min;
	numCells_x = (int)(range.x / h);
	numCells_y = (int)(range.y / h);
	numCells_z = (int)(range.z / h);
	assert(numCells_x >= 1 && numCells_y >= 1 && numCells_z >= 1);
	numCells = numCells_x*numCells_y*numCells_z;

	delta.x = range.x / numCells_x;
	delta.y = range.y / numCells_y;
	delta.z = range.z / numCells_z;
	assert(delta.x >= h && delta.y >= h && delta.z >= h);
	
	assert(sizeof(Cell) % c_cacheline_size == 0);
	assert(offsetof(Cell_aux, padding) == offsetof(Cell, padding));

	cells = (Cell *)_mm_malloc(sizeof(Cell)*numCells, c_cacheline_size);
	cells2 = (Cell *)_mm_malloc(sizeof(Cell)*numCells, c_cacheline_size);
	cell_num_particles = (int *)_mm_malloc(sizeof(int)*numCells, c_cacheline_size);
	cell_num_particles2 = (int *)_mm_malloc(sizeof(int)*numCells, c_cacheline_size);
	last_cells = (Cell **)_mm_malloc(sizeof(Cell *) * numCells, c_cacheline_size);
	
	assert(cells && cells2 && cell_num_particles && cell_num_particles2);

	for(int i=0; i<numCells; ++i) {
		new (cells+i) Cell;
		new (cells2+i) Cell;
	}

	memset(cell_num_particles, 0, numCells*sizeof(int));

	for(int i = 0; i < num_particles; ++i) {
		pardata pd = get_particle();
		Vec3 p,hv,v;
		p = Vec3(pd.px, pd.py, pd.pz);
		hv = Vec3(pd.hvx, pd.hvy, pd.hvz);
		v = Vec3(pd.vx, pd.vy, pd.vz);

		int index = get_cell_index(p.x, p.y, p.z);
		Cell *cell = cells+index;

		int num_part = cell_num_particles[index];
		// go to last overflow cell in list
		// cell.next will always be valid due to "if" statement underneath
		while(num_part > c_particles_per_cell) {
			cell = cell->next;
			num_part -= c_particles_per_cell;
		}
		// if cell is full, get a cell from the overflow pool
		if((num_part % c_particles_per_cell == 0) && (cell_num_particles[index] != 0)) {
			cell->next = pool.get_cell();
			cell = cell->next;
			num_part -= c_particles_per_cell;
		}
		// add particle to cell
		cell->p[num_part] = p;
		cell->hv[num_part] = hv;
		cell->v[num_part] = v;

		++cell_num_particles[index];
	}
	
	close_read_file();
	
	float pi = static_cast<float>(M_PI);
	float coeff1 = 315.f / (64.f*pi*pow(h,9.f));
	float coeff2 = 15.f / (pi*pow(h,6.f));
	float coeff3 = 45.f / (pi*pow(h,6.f));
	float particleMass = 0.5f*c_double_rest_density / (restParticlesPerMeter*restParticlesPerMeter*restParticlesPerMeter);
	densityCoeff = particleMass * coeff1;
	pressureCoeff = 3.f*coeff2 * 0.5f*c_stiffness_pressure * particleMass;
	viscosityCoeff = c_viscosity * coeff3 * particleMass;
}

// exchanges the buffer cells by swapping cells and cells2
// recalculates which particles go into which cells
//
// [in]: cells2AN, cell_num_particles2AN
// [out]: cellsAN, cell_num_particlesAN
void rebuild_grid() {
	// swap src and dest arrays (almost like switching screen buffers)
	swap(cells, cells2);
	swap(cell_num_particles, cell_num_particles2);

	// Initialize dest
	memset(cell_num_particles, 0, numCells*sizeof(int));
	for(int i=0; i<numCells; ++i) {
		cells[i].next = NULL;
		last_cells[i] = cells+i;
	}

	// iterate through src cells
	for(int i = 0; i < numCells; ++i) {
		Cell *cell2 = cells2+i;
		for(int j = 0; j < cell_num_particles2[i]; ++j) {
			// get last pointer in correct dest cells
			int index = get_cell_index(cell2->p[j%c_particles_per_cell].x, cell2->p[j%c_particles_per_cell].y, cell2->p[j%c_particles_per_cell].z);
			Cell *cell = last_cells[index];
			int part = cell_num_particles[index];

			// grab an overflow cell if necessary
			if((part % c_particles_per_cell == 0) && (cell_num_particles[index] != 0)) {
				cell->next = pool.get_cell();
				cell = cell->next;
				last_cells[index] = cell;
			}
			++cell_num_particles[index];

			// copy src to dest particle
			cell->p[part%c_particles_per_cell] = cell2->p[j%c_particles_per_cell];
			cell->hv[part%c_particles_per_cell] = cell2->hv[j%c_particles_per_cell];
			cell->v[part%c_particles_per_cell] = cell2->v[j%c_particles_per_cell];

			// move to next overflow src cell if necessary
			if(j%c_particles_per_cell == c_particles_per_cell-1) {
				Cell *temp = cell2;
				cell2 = cell2->next;
				// return overflow cells to pool (and not statically allocated cells)
				if(temp != cells2+i) {
					pool.return_cell(temp);
				}
			}
		}
		// if cell2 is overflow, return to pool
		if(cell2 && (cell2 != cells2+i)) {
			pool.return_cell(cell2);
		}
	}
}

// gets only half of the surrounding neighbor cells, given a certain x,y,z coordinate cell
// This is required so that each combination of particles is only considered once
// fills neigh_cells with indexes of neighbor cells
// returns number of neighbors
int get_neighbor_cells(int cell_x, int cell_y, int cell_z, int *neigh_cells) {
	int num_neigh_cells = 0;

	// get self first
	neigh_cells[num_neigh_cells++] = (cell_z*numCells_y + cell_y) * numCells_x + cell_x;
		
	// find surrounding neighbors
	for(int dx = -1; dx <= 1; ++dx)
		for(int dy = -1; dy <= 1; ++dy)
			for(int dz = -1; dz <= 1; ++dz) {
				int xx = cell_x + dx;
				int yy = cell_y + dy;
				int zz = cell_z + dz;
				if(xx >= 0 && xx < numCells_x && yy >= 0 && yy < numCells_y && zz >= 0 && zz < numCells_z) {
					int index = (zz*numCells_y + yy)*numCells_x + xx;
					// Only add cells lower than current cell so that calculations are only done once per pair
					// Also make sure there are actually particles to do work on
					if((index < neigh_cells[0]) && (cell_num_particles[index] != 0)) {
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
void compute_densities() {
	int cell_index = 0;
	
	for(int cell_z = 0; cell_z < numCells_z; ++cell_z) {
		for(int cell_y = 0; cell_y < numCells_y; ++cell_y) {
			for(int cell_x = 0; cell_x < numCells_x; ++cell_x, ++cell_index) {
				int num_pars = cell_num_particles[cell_index];
				if(num_pars == 0)
					continue;

				int neigh_cells[27];
				int num_neigh_cells = get_neighbor_cells(cell_x, cell_y, cell_z, neigh_cells);

				Cell *cell = cells+cell_index;
				// For each particle in the cell
				for(int par_index = 0; par_index < num_pars; ++par_index) {
					int ipar = par_index%c_particles_per_cell;
					// The first index in neigh_cells is always cell_index
					// Thus, cell = neigh. This requires special treatment
					int neigh_cell_index = neigh_cells[0];
					Cell *neigh = cells+neigh_cell_index;
					// only iterate to par_index to make sure you do not consider a combination twice
					for(int neigh_par_index = 0; neigh_par_index < par_index; ++neigh_par_index) {
						int ineigh_par = neigh_par_index%c_particles_per_cell;
						// Actual kernel calculation for density occurs here
						float distSq = (cell->p[ipar] - neigh->p[ineigh_par]).GetLengthSq();
						if(distSq < hSq) {
							float t = hSq - distSq;
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
						neigh = cells+neigh_cell_index;
						// For each particle in the neighbor cell
						for(int neigh_par_index = 0; neigh_par_index < cell_num_particles[neigh_cell_index]; ++neigh_par_index) {
							int ineigh_par = neigh_par_index%c_particles_per_cell;
							// Actual kernel calculation for density occurs here
							float distSq = (cell->p[ipar] - neigh->p[ineigh_par]).GetLengthSq();
							if(distSq < hSq) {
								float t = hSq - distSq;
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

	const float tc = hSq*hSq*hSq;
	for(int i = 0; i < numCells; ++i) {
		Cell *cell = cells+i;
		for(int j = 0; j < cell_num_particles[i]; ++j) {
			cell->density[j%c_particles_per_cell] += tc;
			cell->density[j%c_particles_per_cell] *= densityCoeff;
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
void compute_accelerations() {
	int cell_index = 0;
	
	for(int cell_z = 0; cell_z < numCells_z; ++cell_z) {
		for(int cell_y = 0; cell_y < numCells_y; ++cell_y) {
			for(int cell_x = 0; cell_x < numCells_x; ++cell_x, ++cell_index) {
				int num_pars = cell_num_particles[cell_index];
				if(num_pars == 0)
					continue;

				int neigh_cells[27];
				int num_neigh_cells = get_neighbor_cells(cell_x, cell_y, cell_z, neigh_cells);

				Cell *cell = cells+cell_index;
				// For each particle in the cell
				for(int par_index = 0; par_index < num_pars; ++par_index) {
					int ipar = par_index%c_particles_per_cell;
					// The first index in neigh_cells is always cell_index
					// Thus, cell = neigh. This requires special treatment
					int neigh_cell_index = neigh_cells[0];
					Cell *neigh = cells+neigh_cell_index;
					// only iterate to par_index to make sure you do not consider a combination twice
					for(int neigh_par_index = 0; neigh_par_index < par_index; ++neigh_par_index) {
						int ineigh_par = neigh_par_index%c_particles_per_cell;
						// Actual kernel calculation for acceleration occurs here
						Vec3 disp = cell->p[ipar] - neigh->p[ineigh_par];
						float distSq = disp.GetLengthSq();
						if(distSq < hSq) {
							float dist = sqrtf(max(distSq, 1e-12f));

							float hmr = h - dist;

							Vec3 acc = disp * pressureCoeff * (hmr*hmr/dist) * (cell->density[ipar]+neigh->density[ineigh_par] - c_double_rest_density);
							acc += (neigh->v[ineigh_par] - cell->v[ipar]) * viscosityCoeff * hmr;
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
						neigh = cells+neigh_cell_index;
						// For each particle in the neighbor cell
						for(int neigh_par_index = 0; neigh_par_index < cell_num_particles[neigh_cell_index]; ++neigh_par_index) {
							int ineigh_par = neigh_par_index%c_particles_per_cell;
							// Actual kernel calculation for acceleration occurs here
							Vec3 disp = cell->p[ipar] - neigh->p[ineigh_par];
							float distSq = disp.GetLengthSq();
							if(distSq < hSq) {
								float dist = sqrtf(max(distSq, 1e-12f));

								float hmr = h - dist;

								Vec3 acc = disp * pressureCoeff * (hmr*hmr/dist) * (cell->density[ipar]+neigh->density[ineigh_par] - c_double_rest_density);
								acc += (neigh->v[ineigh_par] - cell->v[ipar]) * viscosityCoeff * hmr;
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
void compute_forces() {
	for(int i = 0; i < numCells; ++i) {
		Cell *cell = cells+i;
		for(int j = 0; j < cell_num_particles[i]; ++j) {
			cell->density[j%c_particles_per_cell] = 0.f;
			cell->a[j%c_particles_per_cell] = c_external_acceleration;
			// move pointer to next overflow cell if end of current cell reached
			if(j % c_particles_per_cell == c_particles_per_cell-1) {
				cell=cell->next;
			}
		}
	}

	compute_densities();
	compute_accelerations();
}


// general function to check if particle escapes from a minimum wall and prevent it
// works for either min x, y, or z
// takes advantage of the structure of a class. Vec3.x, Vec3.y, and Vec3.z are in contiguous memory
// 
// [in]: x,y,z cell coordinate, index (either 0,1,or 2 for x,y,z)
// [out]: updated acceleration
void check_if_past_min_wall(int x, int y, int z, int index) {
	int cwall = (z*numCells_y + y)*numCells_x + x;
	Cell *cell = cells+cwall;
	for(int i=0; i<cell_num_particles[cwall]; ++i) {
		int i_index = i % c_particles_per_cell;
		float cell_p = (&(cell->p[i_index].x))[index];
		float cell_hv = (&(cell->hv[i_index].x))[index];
		float pos_on_wall = cell_p + cell_hv * c_time_step;
		const float idomain_min = (&c_domain_min.x)[index];
		float diff = c_par_size - (pos_on_wall - idomain_min);
		if (diff > c_epsilon) {
			float &cell_a = (&(cell->a[i_index].x))[index];
			float cell_v = (&(cell->v[i_index].x))[index];
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
// takes advantage of the structure of a class. Vec3.x, Vec3.y, and Vec3.z are in contiguous memory
//
// [in]: x,y,z cell coordinate, index (either 0,1,or 2 for x,y,z)
// [out]: updated acceleration
void check_if_past_max_wall(int x, int y, int z, int index) {
	int cwall = (z*numCells_y + y)*numCells_x + x;
	Cell *cell = cells+cwall;
	for(int i=0; i<cell_num_particles[cwall]; ++i) {
		int i_index = i % c_particles_per_cell;
		float cell_p = (&(cell->p[i_index].x))[index];
		float cell_hv = (&(cell->hv[i_index].x))[index];
		float pos_on_wall = cell_p + cell_hv * c_time_step;
		const float idomain_max = (&c_domain_max.x)[index];
		float diff = c_par_size - (idomain_max - pos_on_wall);
		if (diff > c_epsilon) {
			float &cell_a = (&(cell->a[i_index].x))[index];
			float cell_v = (&(cell->v[i_index].x))[index];
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
void process_collisions() {
	int x,y,z;
	x=0;  // along the c_domain_min.x wall
	int xmax=numCells_x-1;  // along the c_domain_max.x wall
	for(y=0; y<numCells_y; ++y) {
		for(z=0; z<numCells_z; ++z) {
			check_if_past_min_wall(x,y,z,0);
			check_if_past_max_wall(xmax,y,z,0);
		}
	}

	y=0; // along the c_domain_min.y wall
	int ymax=numCells_y-1; // along the c_domain_max.y wall
	for(x=0; x<numCells_x; ++x) {
		for(z=0; z<numCells_z; ++z) {
			check_if_past_min_wall(x,y,z,1);
			check_if_past_max_wall(x,ymax,z,1);
		}
	}

	z=0;  // along the c_domain_min.z wall
	int zmax=numCells_z-1;  // along the c_domain_max.z wall
	for(x=0; x<numCells_x; ++x) {
		for(y=0; y<numCells_y; ++y) {
			check_if_past_min_wall(x,y,z,2);
			check_if_past_max_wall(x,y,zmax,2);
		}
	}
}

// second pass over the minimum edges of the domain
// some particles could be moved out of bounds and then printed to the screen otherwise
// To be called after advance_particles
//
// [in]: x,y,z cell coordinate, index (either 0,1,or 2 for x,y,z)
// [out]: updated p, v, and hv
void check_if_past_min_wall2(int x, int y, int z, int index) {
	int cwall = (z*numCells_y + y)*numCells_x + x;
	Cell *cell = cells+cwall;
	for(int i=0; i<cell_num_particles[cwall]; ++i) {
		int i_index = i % c_particles_per_cell;
		float &cell_p = (&(cell->p[i_index].x))[index];
		const float idomain_min = (&(c_domain_min.x))[index];
		float diff = cell_p - idomain_min;
		if(diff < 0.f) {
			float &cell_v = (&(cell->v[i_index].x))[index];
			float &cell_hv = (&(cell->hv[i_index].x))[index];
			cell_p = idomain_min - diff;
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
void check_if_past_max_wall2(int x, int y, int z, int index) {
	int cwall = (z*numCells_y + y)*numCells_x + x;
	Cell *cell = cells+cwall;
	for(int i=0; i<cell_num_particles[cwall]; ++i) {
		int i_index = i % c_particles_per_cell;
		float &cell_p = (&(cell->p[i_index].x))[index];
		const float idomain_max = (&(c_domain_max.x))[index];
		float diff = idomain_max - cell_p;
		if(diff < 0.f) {
			float &cell_v = (&(cell->v[i_index].x))[index];
			float &cell_hv = (&(cell->hv[i_index].x))[index];
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
void process_collisions2() {
	int x,y,z;
	x=0;// along the c_domain_min.x wall
	int xmax = numCells_x-1;  // along the c_domain_max.x wall
	for(y=0; y<numCells_y; ++y) {
		for(z=0; z<numCells_z; ++z) {
			check_if_past_min_wall2(x,y,z,0);
			check_if_past_max_wall2(xmax,y,z,0);
		}
	}

	y=0;  // along the c_domain_min.y wall
	int ymax=numCells_y-1;  // along the c_domain_max.y wall
	for(x=0; x<numCells_x; ++x) {
		for(z=0; z<numCells_z; ++z) {
			check_if_past_min_wall2(x,y,z,1);
			check_if_past_max_wall2(x,ymax,z,1);
		}
	}

	z=0;  // along the c_domain_min.z wall
	int zmax=numCells_z-1;  // along the c_domain_max.z wall
	for(x=0; x<numCells_x; ++x) {
		for(y=0; y<numCells_y; ++y) {
			check_if_past_min_wall2(x,y,z,2);
			check_if_past_max_wall2(x,y,zmax,2);
		}
	}
}

// advances particle positions based on their hv (half velocity)
// updates velocity based on calculated acceleration
// [in]: cell.hv, cell.a
// [out]: cell.p, cell.v, cell.hv
void advance_particles() {
	for(int i = 0; i < numCells; ++i) {
		Cell *cell = cells+i;
		for(int j = 0; j < cell_num_particles[i]; ++j) {
			Vec3 v_half = cell->hv[j % c_particles_per_cell] + cell->a[j % c_particles_per_cell]*c_time_step;
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
// uses all linear/scalar function kernels
void advance_frame() {
	rebuild_grid();
	compute_forces();
	// updates acceleration
	process_collisions();
	advance_particles();
	// fixes any particle position that moved across domain
	process_collisions2();
}


// Delete all cells and cellpool
void clean_up_sim()
{
	// first return all overflow cells to the pool
	for(int i=0; i<numCells; ++i) {
		Cell& cell = cells[i];
		while(cell.next) {
			Cell *temp = cell.next;
			cell.next = temp->next;
			pool.return_cell(temp);
		}
	}
	// normally you could just reuse this, but to keep memory work consistent across runs, explicitly free the pool
	pool.~cellpool();
	_mm_free(cells);
	_mm_free(cells2);
	_mm_free(cell_num_particles);
	_mm_free(cell_num_particles2);
	_mm_free(last_cells);
}

// Save particles to file if necessary, cleans up allocated cell information
void end_sim(string fileName) {
	if(!fileName.empty()) {
		open_save_file(fileName);
		save_RPPM_and_numPart();
		for(int i = 0; i < numCells; ++i) {
			Cell *cell = cells+i;
			for(int j = 0; j < cell_num_particles[i]; ++j) {
				save_particle(cell->p[j % c_particles_per_cell].x, cell->p[j % c_particles_per_cell].y, cell->p[j % c_particles_per_cell].z,
					cell->hv[j % c_particles_per_cell].x, cell->hv[j % c_particles_per_cell].y, cell->hv[j % c_particles_per_cell].z,
					cell->v[j % c_particles_per_cell].x, cell->v[j % c_particles_per_cell].y, cell->v[j % c_particles_per_cell].z);

				// move to next overflow cell if end of the current cell
				if(j % c_particles_per_cell == c_particles_per_cell -1) {
					cell = cell->next;
				}
			}
		}
		close_save_file();
	}
	clean_up_sim();
}
