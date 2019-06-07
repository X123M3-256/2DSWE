#ifndef RENDER_H_INCLUDED
#define RENDER_H_INCLUDED
#define GL_SILENCE_DEPRECATION
#include<gl.h>


typedef struct
{
int n;
float size;
float delta_x;
float color[3];
int num_vertices;
int num_indices;
GLuint vao;
GLuint vbo; 
GLuint ibo; 
GLuint texture;
}heightmap_t;

GLuint shader_build(const char* vertex_filename,const char* fragment_filename);

void init_render();
void heightmap_init(heightmap_t* heightmap,int n,float dx,float r,float g,float b);
void heightmap_update(heightmap_t* heightmap,float* data);
void heightmap_render(heightmap_t* heightmap,const float* projection);



#endif // RENDER_H_INCLUDED
