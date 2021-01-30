#ifndef RENDER_H_INCLUDED
#define RENDER_H_INCLUDED
#define GL_SILENCE_DEPRECATION

#include<GL/glew.h>
#include<GL/gl.h>


typedef struct
{
int n;
float size;
float x_offset;
float y_offset;
float z_offset;
float delta_x;
int num_vertices;
int num_indices;
GLuint vao;
GLuint vbo; 
GLuint ibo; 
GLuint height_texture;
GLuint texture;
}heightmap_t;

typedef struct
{
int n;
float size;
float x_offset;
float y_offset;
float z_offset;
float delta_x;
float u;
float* uvs[2];
int num_vertices;
int num_indices;
GLuint vao;
GLuint vbo; 
GLuint uvbo[2]; 
GLuint ibo; 
GLuint height_texture;
GLuint texture;
}water_t;


GLuint shader_build(const char* vertex_filename,const char* fragment_filename);

void init_render();
void heightmap_init(heightmap_t* heightmap,int n,float size,float x_offset,float y_offset,float z_offset,const char* filename);
void heightmap_update(heightmap_t* heightmap,float* data);
void heightmap_render(heightmap_t* heightmap,const float* projection);

void water_init(water_t* water,int n,float size,float x_offset,float y_offset,float z_offset,const char* filename);
void water_update(water_t* water,float* data,float* velocity_x,float* velocity_y,float dt);
void water_render(water_t* water,const float* projection);


#endif // RENDER_H_INCLUDED
