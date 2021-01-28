#ifndef SOLVER_H_INCLUDED
#define SOLVER_H_INCLUDED
#include<stdint.h>

typedef struct
{
float* w;
float* qx;
float* qy;
}cell_data_t;

typedef struct
{
float* w;
float* h;
float* qx;
float* qy;
}edge_data_t;

typedef struct
{
uint32_t x_points;
uint32_t y_points;
float delta_x;

cell_data_t cells;
edge_data_t x_plus;
edge_data_t x_minus;
edge_data_t y_plus;
edge_data_t y_minus;

float* bed;
float* x_flux_w;
float* x_flux_qx;
float* x_flux_qy;
float* y_flux_w;
float* y_flux_qx;
float* y_flux_qy;

}solver_t;

void solver_init(solver_t* solver,uint32_t x_points,uint32_t y_points,float width);
void solver_compute_step(solver_t* s,float dt);
#endif // SOLVER_H_INCLUDED
