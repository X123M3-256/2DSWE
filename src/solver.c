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
solver->delta_x=width/(x_points-2);

solver->cells.w=calloc(x_points*y_points,sizeof(float));
solver->cells.qx=calloc(x_points*y_points,sizeof(float));
solver->cells.qy=calloc(x_points*y_points,sizeof(float));
solver->bed=calloc((x_points-1)*(y_points-1),sizeof(float));


	for(int32_t i=0;i<x_points-1;i++)
	for(int32_t j=0;j<y_points-1;j++)
	{
	float x=i*solver->delta_x;
	float y=j*solver->delta_x;
		//solver->bed[i+j*(solver->x_points-1)]=3*(x/50.0)+4;
//		if(y>20&&y<30)solver->bed[i+j*(solver->x_points-1)]-=3;
//		if(y>17.5&&y<20)solver->bed[i+j*(solver->x_points-1)]-=3*0.4*(y-17.5);
//		if(y>30&&y<32.5)solver->bed[i+j*(solver->x_points-1)]-=3*(1.0-0.4*(y-30.0));

		if(x>30&&x<40)solver->bed[i+j*(solver->x_points-1)]=2.2+2.2*cos(0.2*M_PI*(x-35));//+(y-2.5)*(y-2.5)));
		if(y>17.5&&y<22.5)solver->bed[i+j*(solver->x_points-1)]*=0.5-0.5*cos(0.4*M_PI*(y-20));
		if(y>22.5&&y<27.5)solver->bed[i+j*(solver->x_points-1)]*=0.5-0.5*cos(0.4*M_PI*(y-25));
		if(y>27.5&&y<32.5)solver->bed[i+j*(solver->x_points-1)]*=0.5-0.5*cos(0.4*M_PI*(y-30));
	}
	for(int32_t i=1;i<x_points-1;i++)
	for(int32_t j=1;j<y_points-1;j++)
	{
	float x=(i-0.5)*solver->delta_x;
	float y=(j-0.5)*solver->delta_x;
	float bed=0.25*(solver->bed[i+j*(solver->x_points-1)]+solver->bed[(i-1)+j*(solver->x_points-1)]+solver->bed[i+(j-1)*(solver->x_points-1)]+solver->bed[(i-1)+(j-1)*(solver->x_points-1)]);
	//float level=3.0*(x/50.0)+2.5;
	float level=x>35?4:1;
	solver->cells.w[i+j*x_points]=bed>level?bed:level;
	}

solver_edge_init(&(solver->x_plus),x_points-1,y_points-2);
solver_edge_init(&(solver->x_minus),x_points-1,y_points-2);
solver_edge_init(&(solver->y_plus),x_points-2,y_points-1);
solver_edge_init(&(solver->y_minus),x_points-2,y_points-1);

solver->x_flux_w=calloc((x_points-1)*(y_points-2),sizeof(float));
solver->x_flux_qx=calloc((x_points-1)*(y_points-2),sizeof(float));
solver->x_flux_qy=calloc((x_points-1)*(y_points-2),sizeof(float));
solver->y_flux_w=calloc((x_points-2)*(y_points-1),sizeof(float));
solver->y_flux_qx=calloc((x_points-2)*(y_points-1),sizeof(float));
solver->y_flux_qy=calloc((x_points-2)*(y_points-1),sizeof(float));
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
#define Y_EDGE_MINUS(x,y) (bottom_values[(x)+(y)*(x_points-2)])
#define Y_EDGE_PLUS(x,y) (top_values[(x)+(y)*(x_points-2)])

	for(uint32_t y=1;y<y_points-1;y++)
	for(uint32_t x=1;x<x_points-1;x++)
	{
	float u_x=minmod((CELL(x,y)-CELL(x-1,y))/delta_x,(CELL(x+1,y)-CELL(x,y))/delta_x);
	X_EDGE_MINUS(x,y-1)=CELL(x,y)+0.5*delta_x*u_x;
	X_EDGE_PLUS(x-1,y-1)=CELL(x,y)-0.5*delta_x*u_x;

	float u_y=minmod((CELL(x,y)-CELL(x,y-1))/delta_x,(CELL(x,y+1)-CELL(x,y))/delta_x);
	Y_EDGE_MINUS(x-1,y)=CELL(x,y)+0.5*delta_x*u_y;
	Y_EDGE_PLUS(x-1,y-1)=CELL(x,y)-0.5*delta_x*u_y;
	}

	//TODO not exactly sure what's going on here	
	for(uint32_t x=1;x<x_points-1;x++)
	{
	Y_EDGE_MINUS(x-1,0)=0.5*(CELL(x,0)+CELL(x,1));
	Y_EDGE_PLUS(x-1,y_points-2)=0.5*(CELL(x,y_points-2)+CELL(x,y_points-1));
	}
	
	for(uint32_t y=1;y<y_points-1;y++)
	{
	X_EDGE_MINUS(0,y-1)=0.5*(CELL(0,y)+CELL(1,y));
	X_EDGE_PLUS(x_points-2,y-1)=0.5*(CELL(x_points-2,y)+CELL(x_points-1,y));
	}

#undef CELL
#undef X_EDGE_MINUS
#undef X_EDGE_PLUS
#undef Y_EDGE_MINUS
#undef Y_EDGE_PLUS
}

float x_flux_w(float h,float qx,float qy,float vx,float vy)
{
return qx;
}

float x_flux_qx(float h,float qx,float qy,float vx,float vy)
{
return qx*vx+0.5*9.81*h*h;
}

float x_flux_qy(float h,float qx,float qy,float vx,float vy)
{
return qx*vy;
}

float y_flux_w(float h,float qx,float qy,float vx,float vy)
{
return qy;
}

float y_flux_qx(float h,float qx,float qy,float vx,float vy)
{
return qy*vx;
}

float y_flux_qy(float h,float qx,float qy,float vx,float vy)
{
return qy*vy+0.5*9.81*h*h;
}

#define CELL(u,x,y) ((u)[(x)+(y)*s->x_points])
#define BED(x,y) (s->bed[(x)+(y)*(s->x_points-1)])
#define X_EDGE(u,x,y) ((u)[(x)+(y)*(s->x_points-1)])
#define Y_EDGE(u,x,y) ((u)[(x)+(y)*(s->x_points-2)])

void compute_boundary_conditions(solver_t* s)
{
static float t=0;
	for(uint32_t x=1;x<s->x_points-1;x++)
	{
	float bed=0.5*(BED(x,0)+BED(x-1,0));
	float level=((x-0.5)*s->delta_x)>35?4:1;
	CELL(s->cells.w,x,0)=level>bed?level:bed;//*CELL(s->cells.w,x,1);
	CELL(s->cells.qx,x,0)=0;//CELL(s->cells.qx,x,1);
	CELL(s->cells.qy,x,0)=CELL(s->cells.qy,x,1);

	bed=0.5*(BED(x,s->y_points-2)+BED(x-1,s->y_points-2));
	CELL(s->cells.w,x,s->y_points-1)=level>bed?level:bed;//CELL(s->cells.w,x,s->y_points-2);
	CELL(s->cells.qx,x,s->y_points-1)=0;//CELL(s->cells.qx,x,s->y_points-2);
	CELL(s->cells.qy,x,s->y_points-1)=CELL(s->cells.qy,x,s->y_points-2);
	}
	
	for(uint32_t y=1;y<s->y_points-1;y++)
	{
	float bed=0.5*(BED(0,y)+BED(0,y-1));
	float level=1;//2;
	CELL(s->cells.w,0,y)=level>bed?level:bed;//;//CELL(s->cells.w,1,y);
	CELL(s->cells.qx,0,y)=CELL(s->cells.qx,1,y);
	CELL(s->cells.qy,0,y)=CELL(s->cells.qy,1,y);

	bed=0.5*(BED(s->x_points-2,y)+BED(s->x_points-2,y-1));
	level=4;//5.5;//+0.04*sin(t);
	CELL(s->cells.w,s->x_points-1,y)=level>bed?level:bed;//0.5*(BED(s->x_points-2,y)+BED(s->x_points-2,y));//CELL(s->cells.w,s->x_points-2,y);
	CELL(s->cells.qx,s->x_points-1,y)=CELL(s->cells.qx,s->x_points-2,y);
	CELL(s->cells.qy,s->x_points-1,y)=CELL(s->cells.qy,s->x_points-2,y);
	}

t+=0.03;
}

float compute_velocity(float q,float h)
{
#define EPSILON 0.0000001
float h4=h*h*h*h;
return M_SQRT2*h*q/sqrt(h4+max(h4,EPSILON));
}

float safe_sqrt(float x)
{
	if(x<0)return 0;
return sqrt(x);
}

void compute_fluxes(solver_t* s)
{
//X flux
	for(uint32_t y=0;y<s->y_points-2;y++)
	for(uint32_t x=0;x<s->x_points-1;x++)
	{
	float bed=0.5*(BED(x,y)+BED(x,y+1));
	float w_plus=X_EDGE(s->x_plus.w,x,y);
	float h_plus=w_plus-bed;
		if(h_plus<-0.00001)printf("HNEG X+ %f coords %d %d\n",h_plus,x,y);
	float qx_plus=X_EDGE(s->x_plus.qx,x,y);
	float qy_plus=X_EDGE(s->x_plus.qy,x,y);
	float w_minus=X_EDGE(s->x_minus.w,x,y);
	float h_minus=w_minus-bed;
		if(h_minus<-0.00001)printf("HNEG X- %f coords %d %d\n",h_minus,x,y);
	float qx_minus=X_EDGE(s->x_minus.qx,x,y);
	float qy_minus=X_EDGE(s->x_minus.qy,x,y);

	float vx_plus=compute_velocity(qx_plus,h_plus);
	float vx_minus=compute_velocity(qx_minus,h_minus);
	float vy_plus=compute_velocity(qy_plus,h_plus);
	float vy_minus=compute_velocity(qy_minus,h_minus);
	qx_plus=vx_plus*h_plus;
	qx_minus=vx_minus*h_minus;
	qy_plus=vy_plus*h_plus;
	qy_minus=vy_minus*h_minus;
	float c_plus=safe_sqrt(9.81*h_plus);
	float c_minus=safe_sqrt(9.81*h_minus);
	float a_plus=max(max(vx_minus+c_minus,vx_plus+c_plus),0);
	float a_minus=min(min(vx_minus-c_minus,vx_plus-c_plus),0);
		
		if(a_plus!=a_minus)
		{	
		X_EDGE(s->x_flux_w ,x,y)=(a_plus*x_flux_w (h_minus,qx_minus,qy_minus,vx_minus,vy_minus)-a_minus*x_flux_w (h_plus,qx_plus,qy_plus,vx_plus,vy_plus)+a_minus*a_plus*(w_plus-w_minus))/(a_plus-a_minus);
		X_EDGE(s->x_flux_qx,x,y)=(a_plus*x_flux_qx(h_minus,qx_minus,qy_minus,vx_minus,vy_minus)-a_minus*x_flux_qx(h_plus,qx_plus,qy_plus,vx_plus,vy_plus)+a_minus*a_plus*(qx_plus-qx_minus))/(a_plus-a_minus);
		X_EDGE(s->x_flux_qy,x,y)=(a_plus*x_flux_qy(h_minus,qx_minus,qy_minus,vx_minus,vy_minus)-a_minus*x_flux_qy(h_plus,qx_plus,qy_plus,vx_plus,vy_plus)+a_minus*a_plus*(qy_plus-qy_minus))/(a_plus-a_minus);
		}
		else
		{
		X_EDGE(s->x_flux_w,x,y)=0.0;
		X_EDGE(s->x_flux_qx,x,y)=0.0;
		X_EDGE(s->x_flux_qy,x,y)=0.0;
		}
		/*
		if(x==s->x_points-2&&y==50)
		{
		printf("h: %f %f\n",h_plus,h_minus);
		printf("qx: %f %f\n",qx_plus,qx_minus);
		printf("qy: %f %f\n",qy_plus,qy_minus);
		printf("%f\n\n",X_EDGE(s->x_flux_w ,x,y));
			if(isnan(h_plus))exit(0);
		}
		*/
	}

//Y flux

	for(uint32_t y=0;y<s->y_points-1;y++)
	for(uint32_t x=0;x<s->x_points-2;x++)
	{
	float bed=0.5*(BED(x,y)+BED(x+1,y));
	float w_plus=Y_EDGE(s->y_plus.w,x,y);
	float h_plus=w_plus-bed;
	//	if(h_plus<0)printf("HNEG\n");
	float qx_plus=Y_EDGE(s->y_plus.qx,x,y);
	float qy_plus=Y_EDGE(s->y_plus.qy,x,y);
	float w_minus=Y_EDGE(s->y_minus.w,x,y);
	float h_minus=w_minus-bed;
	//	if(h_minus<0)printf("HNEG\n");
	float qx_minus=Y_EDGE(s->y_minus.qx,x,y);
	float qy_minus=Y_EDGE(s->y_minus.qy,x,y);

	float vx_plus=compute_velocity(qx_plus,h_plus);
	float vx_minus=compute_velocity(qx_minus,h_minus);
	float vy_plus=compute_velocity(qy_plus,h_plus);
	float vy_minus=compute_velocity(qy_minus,h_minus);
	qx_plus=vx_plus*h_plus;
	qx_minus=vx_minus*h_minus;
	qy_plus=vy_plus*h_plus;
	qy_minus=vy_minus*h_minus;
	float c_plus=safe_sqrt(9.81*h_plus);
	float c_minus=safe_sqrt(9.81*h_minus);
	float a_plus=max(max(vy_minus+c_minus,vy_plus+c_plus),0);
	float a_minus=min(min(vy_minus-c_minus,vy_plus-c_plus),0);
		
		if(a_plus!=a_minus)
		{
		Y_EDGE(s->y_flux_w ,x,y)=(a_plus*y_flux_w (h_minus,qx_minus,qy_minus,vx_minus,vy_minus)-a_minus*y_flux_w (h_plus,qx_plus,qy_plus,vx_plus,vy_plus)+a_minus*a_plus*(w_plus-w_minus))/(a_plus-a_minus);
		Y_EDGE(s->y_flux_qx,x,y)=(a_plus*y_flux_qx(h_minus,qx_minus,qy_minus,vx_minus,vy_minus)-a_minus*y_flux_qx(h_plus,qx_plus,qy_plus,vx_plus,vy_plus)+a_minus*a_plus*(qx_plus-qx_minus))/(a_plus-a_minus);
		Y_EDGE(s->y_flux_qy,x,y)=(a_plus*y_flux_qy(h_minus,qx_minus,qy_minus,vx_minus,vy_minus)-a_minus*y_flux_qy(h_plus,qx_plus,qy_plus,vx_plus,vy_plus)+a_minus*a_plus*(qy_plus-qy_minus))/(a_plus-a_minus);
		
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
//static int count=0;
//	if(count%10==0)printf("%d\n",count);
//	if(count>600)getchar();
//count++;
//Impose boundary conditions
compute_boundary_conditions(s);

//Compute reconstructed left and right values at the cell boundaries
compute_reconstruction(s->x_points,s->y_points,s->delta_x,s->cells.w ,s->x_minus.w ,s->x_plus.w ,s->y_minus.w, s->y_plus.w);
compute_reconstruction(s->x_points,s->y_points,s->delta_x,s->cells.qx,s->x_minus.qx,s->x_plus.qx,s->y_minus.qx,s->y_plus.qx);
compute_reconstruction(s->x_points,s->y_points,s->delta_x,s->cells.qy,s->x_minus.qy,s->x_plus.qy,s->y_minus.qy,s->y_plus.qy);

//Correct values of w to ensure positivity
	for(uint32_t y=1;y<s->y_points-1;y++)
	for(uint32_t x=1;x<s->x_points-1;x++)
	{
	float bed_n=0.5*(BED(x,y)+BED(x-1,y));
	float bed_s=0.5*(BED(x,y-1)+BED(x-1,y-1));
	float bed_e=0.5*(BED(x,y)+BED(x,y-1));
	float bed_w=0.5*(BED(x-1,y)+BED(x-1,y-1));
	
		if(X_EDGE(s->x_minus.w,x,y-1)<bed_e)
		{
		X_EDGE(s->x_minus.w,x,y-1)=bed_e;
			if(X_EDGE(s->x_plus.w,x-1,y-1)<bed_w)X_EDGE(s->x_plus.w,x-1,y-1)=bed_w;
			else X_EDGE(s->x_plus.w,x-1,y-1)=2*CELL(s->cells.w,x,y)-bed_e;
		}
		else if(X_EDGE(s->x_plus.w,x-1,y-1)<bed_w)
		{
		X_EDGE(s->x_minus.w,x,y-1)=2*CELL(s->cells.w,x,y)-bed_w;
		X_EDGE(s->x_plus.w,x-1,y-1)=bed_w;
		}

		if(Y_EDGE(s->y_minus.w,x-1,y)<bed_n)
		{
		Y_EDGE(s->y_minus.w,x-1,y)=bed_n;
			if(Y_EDGE(s->y_plus.w,x-1,y-1)<bed_s)Y_EDGE(s->y_plus.w,x-1,y-1)=bed_s;
			else Y_EDGE(s->y_plus.w,x-1,y-1)=2*CELL(s->cells.w,x,y)-bed_n;
		}
		else if(Y_EDGE(s->y_plus.w,x-1,y-1)<bed_s)
		{
		Y_EDGE(s->y_minus.w,x-1,y)=2*CELL(s->cells.w,x,y)-bed_s;
		Y_EDGE(s->y_plus.w,x-1,y-1)=bed_s;
		}
	}
	
	for(uint32_t x=0;x<s->x_points-2;x++)
	{
	float bed=0.5*(BED(x,0)+BED(x+1,0));
		if(Y_EDGE(s->y_minus.w,x,0)<bed)Y_EDGE(s->y_minus.w,x,0)=bed;
	bed=0.5*(BED(x,s->y_points-2)+BED(x+1,s->y_points-2));
		if(Y_EDGE(s->y_plus.w,x,s->y_points-2)<bed)Y_EDGE(s->y_plus.w,x,s->y_points-2)=bed;
	}
	for(uint32_t y=0;y<s->y_points-2;y++)
	{
	float bed=0.5*(BED(0,y)+BED(0,y+1));
		if(X_EDGE(s->x_minus.w,0,y)<bed)X_EDGE(s->x_minus.w,0,y)=bed;
	bed=0.5*(BED(s->x_points-2,y)+BED(s->x_points-2,y+1));
		if(X_EDGE(s->x_plus.w,s->x_points-2,y)<bed)X_EDGE(s->x_plus.w,s->x_points-2,y)=bed;
	}
	
//Compute fluxes
compute_fluxes(s);
	
//Compute next time step
	for(uint32_t y=1;y<s->y_points-1;y++)
	for(uint32_t x=1;x<s->x_points-1;x++)
	{
	float bed=0.25*(BED(x,y)+BED(x-1,y)+BED(x,y-1)+BED(x-1,y-1));
	CELL(s->cells.w,x,y)+=dt*(X_EDGE(s->x_flux_w,x-1,y-1)-X_EDGE(s->x_flux_w,x,y-1)+Y_EDGE(s->y_flux_w,x-1,y-1)-Y_EDGE(s->y_flux_w,x-1,y))/s->delta_x;
	CELL(s->cells.qx,x,y)+=dt*(X_EDGE(s->x_flux_qx,x-1,y-1)-X_EDGE(s->x_flux_qx,x,y-1)+Y_EDGE(s->y_flux_qx,x-1,y-1)-Y_EDGE(s->y_flux_qx,x-1,y))/s->delta_x-0.5*9.81*dt*(CELL(s->cells.w,x,y)-bed)*((BED(x,y)+BED(x,y-1))-(BED(x-1,y)+BED(x-1,y-1)))/s->delta_x-(s->delta_x*s->delta_x/dt)*0.00001*CELL(s->cells.qx,x,y)/(CELL(s->cells.w,x,y)*CELL(s->cells.w,x,y));
	CELL(s->cells.qy,x,y)+=dt*(X_EDGE(s->x_flux_qy,x-1,y-1)-X_EDGE(s->x_flux_qy,x,y-1)+Y_EDGE(s->y_flux_qy,x-1,y-1)-Y_EDGE(s->y_flux_qy,x-1,y))/s->delta_x-0.5*9.81*dt*(CELL(s->cells.w,x,y)-bed)*((BED(x,y)+BED(x-1,y))-(BED(x,y-1)+BED(x-1,y-1)))/s->delta_x-(s->delta_x*s->delta_x/dt)*0.00001*CELL(s->cells.qy,x,y)/(CELL(s->cells.w,x,y)*CELL(s->cells.w,x,y));

	}

}
