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

		float scale=180.0;

		float x_unit=scale/SCREEN_WIDTH;
		float y_unit=scale/SCREEN_HEIGHT;
		float z_unit=1e-3;

		const float projection[]={
			-sqrt(0.5)*x_unit,0.5*sqrt(0.5)*y_unit,0.25*sqrt(6)*z_unit,0.0f,
			sqrt(0.5)*x_unit,0.5*sqrt(0.5)*y_unit,0.25*sqrt(6)*z_unit,0.0f,
			0.0f,0.5*sqrt(3)*y_unit,-0.5*z_unit,0.0f,
			0.0f,-0.8f,0.0f,1.0f,
			};
		heightmap_t bed,water;
		heightmap_init(&bed,256,5.0,0.5,0.5,0.5);
		heightmap_init(&water,256,5.0,0.0,0.5,1.0);


		solver_t solver;
		solver_init(&solver,256,256,5.0);
		int running=true;
			while(running)
			{
			SDL_Event event;
				while(SDL_PollEvent(&event)!=0)
				{
					if(event.type==SDL_QUIT)running=false;
				}
			glClearColor(0.0, 0.0, 0.0, 1.0);
			glClear(GL_COLOR_BUFFER_BIT);
			solver_compute_step(&solver,0.001);
			solver_compute_step(&solver,0.001);
			heightmap_update(&water,solver.cells.w);
			heightmap_render(&water,projection);
			SDL_GL_SwapWindow(window);
			}

		SDL_GL_DeleteContext(context);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 0;
	}
