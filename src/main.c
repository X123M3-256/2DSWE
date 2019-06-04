#include<stdbool.h>
#include<stdio.h>
#include<math.h>
#include<SDL.h>
#include<GL/gl.h>

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

		SDL_GLContext context=SDL_GL_CreateContext(window);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);	
		
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
			SDL_GL_SwapWindow(window);
			}
		SDL_GL_DeleteContext(context);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 0;
	}
