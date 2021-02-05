#ifndef TEXTURE_H_INCLUDED
#define TEXTURE_H_INCLUDED
#define GL_SILENCE_DEPRECATION

#include<GL/glew.h>
#include<GL/gl.h>


int texture_load(const char* filename);
int texture_load_cubemap(const char* filename);


#endif // TEXTURE_H_INCLUDED
