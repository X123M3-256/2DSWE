#include "solver.h"
#include<stdlib.h>
#include<math.h>

void solver_edge_init(edge_data_t* edge_data,uint32_t x_points,uint32_t y_points)
{
edge_data->w=calloc(x_points*y_points,sizeof(float));
edge_data->h=calloc(x_points*y_points,sizeof(float));
edge_data->qx=calloc(x_points*y_points,sizeof(float));
edge_data->qy=calloc(x_points*y_points,sizeof(float));
}

void solver_init(solver_t* solver,uint32_t x_points,uint32_t y_points,float width)
{
solver->x_points=x_points;
solver->y_points=y_points;
solver->delta_x=width/(x_points-1);

solver->cells.w=calloc(x_points*y_points,sizeof(float));
solver->cells.qx=calloc(x_points*y_points,sizeof(float));
solver->cells.qy=calloc(x_points*y_points,sizeof(float));

	for(int32_t i=0;i<x_points;i++)
	for(int32_t j=0;j<y_points;j++)
	{
	float x=i*solver->delta_x;
	float y=j*solver->delta_x;
	solver->cells.w[i+j*x_points]=1.0/(1.0+2*((x-2.5)*(x-2.5)+(y-2.5)*(y-2.5)));
	}

solver_edge_init(&(solver->x_plus),x_points-1,y_points);
solver_edge_init(&(solver->x_minus),x_points-1,y_points);
solver_edge_init(&(solver->y_plus),x_points,y_points-1);
solver_edge_init(&(solver->y_minus),x_points,y_points-1);

solver->x_flux_w=calloc((x_points-1)*y_points,sizeof(float));
solver->x_flux_qx=calloc((x_points-1)*y_points,sizeof(float));
solver->x_flux_qy=calloc((x_points-1)*y_points,sizeof(float));
solver->y_flux_w=calloc(x_points*(y_points-1),sizeof(float));
solver->y_flux_qx=calloc(x_points*(y_points-1),sizeof(float));
solver->y_flux_qy=calloc(x_points*(y_points-1),sizeof(float));
}


float max(float a,float b)
{
return a>=b?a:b;
}
float min(float a,float b)
{
return a<=b?a:b;
}

float minmod(float left,float right)
{
	if(left*right<0)return 0.0;
	else if(fabs(left)<fabs(right))return left;
	else return right;
}


void compute_reconstruction(uint32_t x_points,uint32_t y_points,float delta_x,float* cell_values,float* left_values,float* right_values,float* bottom_values,float* top_values)
{
#define CELL(x,y) (cell_values[(x)+(y)*x_points])
#define X_EDGE_MINUS(x,y) (left_values[(x)+(y)*(x_points-1)])
#define X_EDGE_PLUS(x,y) (right_values[(x)+(y)*(x_points-1)])
#define Y_EDGE_MINUS(x,y) (bottom_values[(x)+(y)*x_points])
#define Y_EDGE_PLUS(x,y) (top_values[(x)+(y)*x_points])

	for(uint32_t y=0;y<y_points;y++)
	for(uint32_t x=1;x<x_points-1;x++)
	{
	float u_x=minmod((CELL(x,y)-CELL(x-1,y))/delta_x,(CELL(x+1,y)-CELL(x,y))/delta_x);
	X_EDGE_MINUS(x,y)=CELL(x,y)+0.5*delta_x*u_x;
	X_EDGE_PLUS(x-1,y)=CELL(x,y)-0.5*delta_x*u_x;
	}

	for(uint32_t y=1;y<y_points-1;y++)
	for(uint32_t x=0;x<x_points;x++)
	{
	float u_y=minmod((CELL(x,y)-CELL(x,y-1))/delta_x,(CELL(x,y+1)-CELL(x,y))/delta_x);
	Y_EDGE_MINUS(x,y)=CELL(x,y)+0.5*delta_x*u_y;
	Y_EDGE_PLUS(x,y-1)=CELL(x,y)-0.5*delta_x*u_y;
	}
	
	for(uint32_t x=0;x<x_points;x++)
	{
	Y_EDGE_MINUS(x,0)=0.5*(CELL(x,0)+CELL(x,1));
	Y_EDGE_PLUS(x,y_points-2)=0.5*(CELL(x,y_points-2)+CELL(x,y_points-1));
	}
	
	for(uint32_t y=0;y<y_points;y++)
	{
	X_EDGE_MINUS(0,y)=0.5*(CELL(0,y)+CELL(1,y));
	X_EDGE_PLUS(x_points-2,y)=0.5*(CELL(x_points-2,y)+CELL(x_points-1,y));
	}
}

float x_flux_w(float w,float qx,float qy)
{
return qx;
}

float x_flux_qx(float w,float qx,float qy)
{
return qx*qx/w+0.5*9.81*w*w;
}

float x_flux_qy(float w,float qx,float qy)
{
return qx*qy/w;
}

float y_flux_w(float w,float qx,float qy)
{
return qy;
}

float y_flux_qx(float w,float qx,float qy)
{
return qy*qx/w;
}

float y_flux_qy(float w,float qx,float qy)
{
return qy*qy/w+0.5*9.81*w*w;
}

#define CELL(u,x,y) ((u)[(x)+(y)*s->x_points])
#define X_EDGE(u,x,y) ((u)[(x)+(y)*(s->x_points-1)])
#define Y_EDGE(u,x,y) ((u)[(x)+(y)*s->x_points])

void compute_boundary_conditions(solver_t* s)
{
	for(uint32_t x=0;x<s->x_points;x++)
	{
	CELL(s->cells.w,x,0)=CELL(s->cells.w,x,1);
	CELL(s->cells.qx,x,0)=CELL(s->cells.qx,x,1);
	CELL(s->cells.qy,x,0)=-CELL(s->cells.qy,x,1);

	CELL(s->cells.w,x,s->y_points-1)=CELL(s->cells.w,x,s->y_points-2);
	CELL(s->cells.qx,x,s->y_points-1)=CELL(s->cells.qx,x,s->y_points-2);
	CELL(s->cells.qy,x,s->y_points-1)=-CELL(s->cells.qy,x,s->y_points-2);
	}
	
	for(uint32_t y=0;y<s->y_points;y++)
	{
	CELL(s->cells.w,0,y)=CELL(s->cells.w,1,y);
	CELL(s->cells.qx,0,y)=-CELL(s->cells.qx,1,y);
	CELL(s->cells.qy,0,y)=CELL(s->cells.qy,1,y);

	CELL(s->cells.w,s->x_points-1,y)=CELL(s->cells.w,s->x_points-2,y);
	CELL(s->cells.qx,s->x_points-1,y)=-CELL(s->cells.qx,s->x_points-2,y);
	CELL(s->cells.qy,s->x_points-1,y)=CELL(s->cells.qy,s->x_points-2,y);
	}

}

void compute_fluxes(solver_t* s)
{
//X flux

	for(uint32_t y=0;y<s->y_points;y++)
	for(uint32_t x=0;x<s->x_points-1;x++)
	{
	float w_plus=X_EDGE(s->x_plus.w,x,y);
	float qx_plus=X_EDGE(s->x_plus.qx,x,y);
	float qy_plus=X_EDGE(s->x_plus.qy,x,y);
	float w_minus=X_EDGE(s->x_minus.w,x,y);
	float qx_minus=X_EDGE(s->x_minus.qx,x,y);
	float qy_minus=X_EDGE(s->x_minus.qy,x,y);

	float v_plus=qx_plus/w_plus;
	float v_minus=qx_minus/w_minus;
	float c_plus=sqrt(9.81*w_plus);
	float c_minus=sqrt(9.81*w_minus);
	float a_plus=max(max(v_minus+c_minus,v_plus+c_plus),0);
	float a_minus=min(min(v_minus-c_minus,v_plus-c_plus),0);
		
		if(a_plus!=a_minus)
		{	
		X_EDGE(s->x_flux_w ,x,y)=(a_plus*x_flux_w (w_minus,qx_minus,qy_minus)-a_minus*x_flux_w (w_plus,qx_plus,qy_plus)+a_minus*a_plus*(w_plus-w_minus))/(a_plus-a_minus);
		X_EDGE(s->x_flux_qx,x,y)=(a_plus*x_flux_qx(w_minus,qx_minus,qy_minus)-a_minus*x_flux_qx(w_plus,qx_plus,qy_plus)+a_minus*a_plus*(qx_plus-qx_minus))/(a_plus-a_minus);
		X_EDGE(s->x_flux_qy,x,y)=(a_plus*x_flux_qy(w_minus,qx_minus,qy_minus)-a_minus*x_flux_qy(w_plus,qx_plus,qy_plus)+a_minus*a_plus*(qy_plus-qy_minus))/(a_plus-a_minus);
		
		}
		else
		{
		X_EDGE(s->x_flux_w,x,y)=0.0;
		X_EDGE(s->x_flux_qx,x,y)=0.0;
		X_EDGE(s->x_flux_qy,x,y)=0.0;
		}
	}

//Y flux
	for(uint32_t y=0;y<s->y_points-1;y++)
	for(uint32_t x=0;x<s->x_points;x++)
	{
	float w_plus=Y_EDGE(s->y_plus.w,x,y);
	float qx_plus=Y_EDGE(s->y_plus.qx,x,y);
	float qy_plus=Y_EDGE(s->y_plus.qy,x,y);
	float w_minus=Y_EDGE(s->y_minus.w,x,y);
	float qx_minus=Y_EDGE(s->y_minus.qx,x,y);
	float qy_minus=Y_EDGE(s->y_minus.qy,x,y);

	float v_plus=qy_plus/w_plus;
	float v_minus=qy_minus/w_minus;
	float c_plus=sqrt(9.81*w_plus);
	float c_minus=sqrt(9.81*w_minus);
	float a_plus=max(max(v_minus+c_minus,v_plus+c_plus),0);
	float a_minus=min(min(v_minus-c_minus,v_plus-c_plus),0);
		
		if(a_plus!=a_minus)
		{	
		Y_EDGE(s->y_flux_w ,x,y)=(a_plus*y_flux_w (w_minus,qx_minus,qy_minus)-a_minus*y_flux_w (w_plus,qx_plus,qy_plus)+a_minus*a_plus*(w_plus-w_minus))/(a_plus-a_minus);
		Y_EDGE(s->y_flux_qx,x,y)=(a_plus*y_flux_qx(w_minus,qx_minus,qy_minus)-a_minus*y_flux_qx(w_plus,qx_plus,qy_plus)+a_minus*a_plus*(qx_plus-qx_minus))/(a_plus-a_minus);
		Y_EDGE(s->y_flux_qy,x,y)=(a_plus*y_flux_qy(w_minus,qx_minus,qy_minus)-a_minus*y_flux_qy(w_plus,qx_plus,qy_plus)+a_minus*a_plus*(qy_plus-qy_minus))/(a_plus-a_minus);
		
		}
		else
		{
		Y_EDGE(s->y_flux_w,x,y)=0.0;
		Y_EDGE(s->y_flux_qx,x,y)=0.0;
		Y_EDGE(s->y_flux_qy,x,y)=0.0;
		}
	}
}

void solver_compute_step(solver_t* s,float dt)
{
//Impose boundary conditions
compute_boundary_conditions(s);

//Compute numerical derivatives
compute_reconstruction(s->x_points,s->y_points,s->delta_x,s->cells.w ,s->x_minus.w ,s->x_plus.w ,s->y_minus.w, s->y_plus.w);
compute_reconstruction(s->x_points,s->y_points,s->delta_x,s->cells.qx,s->x_minus.qx,s->x_plus.qx,s->y_minus.qx,s->y_plus.qx);
compute_reconstruction(s->x_points,s->y_points,s->delta_x,s->cells.qy,s->x_minus.qy,s->x_plus.qy,s->y_minus.qy,s->y_plus.qy);

//Compute fluxes
compute_fluxes(s);
	
//Compute next time step
	for(uint32_t y=1;y<s->y_points-1;y++)
	for(uint32_t x=1;x<s->x_points-1;x++)
	{
	CELL(s->cells.w,x,y)+=dt*(X_EDGE(s->x_flux_w,x-1,y)-X_EDGE(s->x_flux_w,x,y)+Y_EDGE(s->y_flux_w,x,y-1)-Y_EDGE(s->y_flux_w,x,y))/s->delta_x;
	CELL(s->cells.qx,x,y)+=dt*(X_EDGE(s->x_flux_qx,x-1,y)-X_EDGE(s->x_flux_qx,x,y)+Y_EDGE(s->y_flux_qx,x,y-1)-Y_EDGE(s->y_flux_qx,x,y))/s->delta_x;
	CELL(s->cells.qy,x,y)+=dt*(X_EDGE(s->x_flux_qy,x-1,y)-X_EDGE(s->x_flux_qy,x,y)+Y_EDGE(s->y_flux_qy,x,y-1)-Y_EDGE(s->y_flux_qy,x,y))/s->delta_x;
	}
}
