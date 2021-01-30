#include<stdbool.h>
#include<stdio.h>
#include<math.h>
#include<SDL.h>

#include "render.h"
#include "solver.h"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

	int main(int argc,char* argv[])
	{
		//Initialize SDL
			if(SDL_Init(SDL_INIT_VIDEO) < 0)
			{
			printf("SDL could not initialize! SDL_Error: %s\n",SDL_GetError());
			exit(1);
			}
		SDL_Window* window=SDL_CreateWindow("SDL Tutorial",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,SCREEN_WIDTH,SCREEN_HEIGHT,SDL_WINDOW_OPENGL);
			if(window==NULL)
			{
			printf("Window could not be created! SDL_Error: %s\n",SDL_GetError());
			exit(1);	
			}

		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);	
		SDL_GLContext context=SDL_GL_CreateContext(window);
		GLenum err = glewInit();
		if (GLEW_OK != err)
		{
		printf("GLEW init failed: %s!n", glewGetErrorString(err));
		exit(1);
		}
		init_render();	

		float scale=180.0/10.0;

		float x_unit=scale/SCREEN_WIDTH;
		float y_unit=scale/SCREEN_HEIGHT;
		float z_unit=1e-3;

		const float projection[]={
			-sqrt(0.5)*x_unit,0.5*sqrt(0.5)*y_unit,0.25*sqrt(6)*z_unit,0.0f,
			sqrt(0.5)*x_unit,0.5*sqrt(0.5)*y_unit,0.25*sqrt(6)*z_unit,0.0f,
			0.0f,0.5*sqrt(3)*y_unit,-0.5*z_unit,0.0f,
			0.0f,-0.8f,0.0f,1.0f,
			};
/*
		const float projection[]={
			-sqrt(0.5)*x_unit, 0.5*sqrt(0.5)*y_unit, 0.25*sqrt(6)*z_unit,0.0f,
			 sqrt(0.5)*x_unit, 0.5*sqrt(0.5)*y_unit, 0.25*sqrt(6)*z_unit,0.0f,
			             0.0f,  0.5*sqrt(3)*y_unit,          -0.5*z_unit,0.0f,
			             0.0f,               -0.0f,                 0.0f,1.0f,
			};
*/

		heightmap_t bed;
		water_t water;
		float size=50.0;
		int n=100;

		float dx=size/(n-1);
		heightmap_init(&bed,n-1,size-dx,0,0,0.01,"rock_texture.png");
		water_init(&water,n,size,-0.5*dx,-0.5*dx,0,"water_texture.png");


		float tool_x=2.5;
		float tool_y=2.5;
		float dt=0.002;
		int tool=0;
		float add_rate=0.0;

		solver_t solver;
		solver_init(&solver,n,n,size);
		int running=true;
		float t=0.0;
			while(running)
			{
			SDL_Event event;
				while(SDL_PollEvent(&event)!=0)
				{
					if(event.type==SDL_QUIT)running=false;
					else if(event.type==SDL_MOUSEBUTTONDOWN)
					{
						if(event.button.button==SDL_BUTTON_LEFT)add_rate=0.001;
						else if(event.button.button==SDL_BUTTON_RIGHT)add_rate=-0.001;
					}
					else if(event.type==SDL_MOUSEBUTTONUP)
					{
					add_rate=0.0;	
					}
					else if(event.type==SDL_MOUSEMOTION)
					{
					float screen_x=-1.0+2.0*(event.motion.x/(float)SCREEN_WIDTH);
					float screen_y=-1.0+2.0*((SCREEN_HEIGHT-event.motion.y)/(float)SCREEN_HEIGHT)+0.8;
					tool_x=0.5*sqrt(2)*(2.0*screen_y/y_unit-screen_x/x_unit);
					tool_y=0.5*sqrt(2)*(2.0*screen_y/y_unit+screen_x/x_unit);
					}
					else if(event.type==SDL_KEYDOWN)
					{
						if(event.key.keysym.sym==SDLK_w)tool=0;	
						else if(event.key.keysym.sym==SDLK_s)tool=1;
					}
	
				}
			glClearColor(0.0, 0.0, 0.0, 1.0);
			glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

			#define BED(x,y) (solver.bed[(x)+(y)*(solver.x_points-1)])
				if(add_rate!=0.0)
				{
					if(tool==0)
					{
						for(uint32_t y=1;y<solver.y_points-1;y++)
						for(uint32_t x=1;x<solver.x_points-1;x++)
						{	
						float a_x=x*solver.delta_x-tool_x-0.5*solver.delta_x;
						float a_y=y*solver.delta_x-tool_y-0.5*solver.delta_x;
						float d=sqrt(a_x*a_x+a_y*a_y);
						float r=5;
							if(d<r)
							{
							solver.cells.w[x+y*solver.x_points]+=add_rate*(1.0+cos(3.141592654*d/r))*solver.delta_x*solver.delta_x/dt;
							}
						}
					}
					else if(tool==1)
					{
						for(uint32_t y=0;y<solver.y_points-1;y++)
						for(uint32_t x=0;x<solver.x_points-1;x++)
						{	
						float a_x=x*solver.delta_x-tool_x;
						float a_y=y*solver.delta_x-tool_y;
						float d=sqrt(a_x*a_x+a_y*a_y);
						float r=3;
							if(d<r)
							{
							float addition=0.3*add_rate*(1.0+cos(3.141592654*d/r))*solver.delta_x*solver.delta_x/dt;
								if(BED(x,y)+addition<0.01)addition=0.01-BED(x,y);
							BED(x,y)+=addition;
								if(addition<0)
								{
								solver.cells.w[x+y*solver.x_points]+=0.25*addition;
								solver.cells.w[x+1+y*solver.x_points]+=0.25*addition;
								solver.cells.w[x+(y+1)*solver.x_points]+=0.25*addition;
								solver.cells.w[x+1+(y+1)*solver.x_points]+=0.25*addition;
								}
							}
						}
					}
					for(uint32_t y=1;y<solver.y_points-1;y++)
					for(uint32_t x=1;x<solver.x_points-1;x++)
					{	
					float bed=0.25*(BED(x,y)+BED(x-1,y)+BED(x,y-1)+BED(x-1,y-1));
						if(solver.cells.w[x+y*solver.x_points]<bed)solver.cells.w[x+y*solver.x_points]=bed;
					}
				}
			
			solver_compute_step(&solver,dt);
			solver_compute_step(&solver,dt);
		
			float* velocity_x=calloc(solver.x_points*solver.y_points,sizeof(float));
			float* velocity_y=calloc(solver.x_points*solver.y_points,sizeof(float));
				for(uint32_t y=1;y<solver.y_points-1;y++)
				for(uint32_t x=1;x<solver.x_points-1;x++)
				{
				float h=solver.cells.w[x+y*solver.x_points]-0.25*(solver.bed[x+y*(solver.x_points-1)]+solver.bed[x-1+y*(solver.x_points-1)]+solver.bed[x+(y-1)*(solver.x_points-1)]+solver.bed[x-1+(y-1)*(solver.x_points-1)]);
					if(h>0.0001)
					{
					velocity_x[x+y*solver.x_points]=solver.cells.qx[x+y*solver.x_points]/h;
					velocity_y[x+y*solver.x_points]=solver.cells.qy[x+y*solver.x_points]/h;
					}
				}
			water_update(&water,solver.cells.w,velocity_x,velocity_y,2*dt);
			
			heightmap_update(&bed,solver.bed);
			free(velocity_x);
			free(velocity_y);

			water_render(&water,projection);
			heightmap_render(&bed,projection);
			SDL_GL_SwapWindow(window);
			t+=2*dt;
			printf("%f\n",t);
			}

		SDL_GL_DeleteContext(context);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 0;
	}
