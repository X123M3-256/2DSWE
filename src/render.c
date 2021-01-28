#include<stdlib.h>
#include<stdio.h>
#include<math.h>
#include<limits.h>
#include "render.h"

struct
{
	struct
	{
	GLuint program;
	GLuint mvp_loc;
	GLuint heightmap_loc;
	GLuint delta_x_loc;
	GLuint map_size_loc;
	GLuint offset_loc;
	GLuint mat_color_loc;
	}
	heightmap_shader;
}
resources;

char* read_file(const char* filename)
{
FILE* file;
file=fopen(filename,"rb");
    if(file==NULL)return NULL;
fseek(file,0,SEEK_END);
long length=ftell(file);
fseek(file,0,SEEK_SET);
char* buf=malloc(length+1);
fread(buf,length,1,file);
fclose(file);
buf[length]=0;
return buf;
}


GLuint shader_compile(GLenum type,const char* filename)
{
GLuint shader=glCreateShader(type);
char* source=read_file(filename);
glShaderSource(shader,1,&source,NULL);
glCompileShader(shader);
free(source);
char error[1024];
glGetShaderInfoLog(shader,1023,NULL,error);
printf("version %s\n",glGetString(GL_SHADING_LANGUAGE_VERSION));
printf("error %s\n",error);
return shader;
}

GLuint shader_build(const char* vertex_filename,const char* fragment_filename)
{
GLuint program=glCreateProgram();
GLuint vertex_shader=shader_compile(GL_VERTEX_SHADER,vertex_filename);
GLuint fragment_shader=shader_compile(GL_FRAGMENT_SHADER,fragment_filename);
glAttachShader(program,vertex_shader);
glAttachShader(program,fragment_shader);
glLinkProgram(program);
return program;
}


void init_render()
{
glEnable(GL_DEPTH_TEST); 
resources.heightmap_shader.program=shader_build("shaders/vertex.glsl","shaders/fragment.glsl");
resources.heightmap_shader.mvp_loc=glGetUniformLocation(resources.heightmap_shader.program,"mvp");
resources.heightmap_shader.heightmap_loc=glGetUniformLocation(resources.heightmap_shader.program,"heightmap");
resources.heightmap_shader.delta_x_loc=glGetUniformLocation(resources.heightmap_shader.program,"delta_x");
resources.heightmap_shader.map_size_loc=glGetUniformLocation(resources.heightmap_shader.program,"map_size");
resources.heightmap_shader.offset_loc=glGetUniformLocation(resources.heightmap_shader.program,"offset");
resources.heightmap_shader.mat_color_loc=glGetUniformLocation(resources.heightmap_shader.program,"mat_color");
}



void heightmap_init(heightmap_t* heightmap,int n,float size,float x_offset,float y_offset,float z_offset,float r,float g,float b)
{
heightmap->n=n;
heightmap->size=size;
heightmap->x_offset=x_offset;
heightmap->y_offset=y_offset;
heightmap->z_offset=z_offset;
heightmap->delta_x=size/(n-1);
heightmap->color[0]=r;
heightmap->color[1]=g;
heightmap->color[2]=b;
glGenVertexArrays(1, &(heightmap->vao));  
glBindVertexArray(heightmap->vao);

glGenBuffers(1,&(heightmap->vbo));
glGenBuffers(1,&(heightmap->ibo));
heightmap->num_vertices=(n*n);
heightmap->num_indices=(6*(n-1)*(n-1));

float* vertices=malloc(2*heightmap->num_vertices*sizeof(float));
GLuint* indices=malloc(heightmap->num_indices*sizeof(GLuint));

//Generate triangles
        for(int y=0;y<n;y++)
        for(int x=0;x<n;x++)
        {
	int index=2*(x+y*n);
        vertices[index]=x*heightmap->delta_x;
        vertices[index+1]=y*heightmap->delta_x;
        }

        for(int y=0;y<n-1;y++)
        for(int x=0;x<n-1;x++)
        {
	int index=6*(x+y*(n-1));
        indices[index]=  x  +  y  *n;
        indices[index+1]=(x+1)+  y  *n;
        indices[index+2]=  x  +(y+1)*n;

        indices[index+3]=(x+1)+  y  *n;
        indices[index+4]=(x+1)+(y+1)*n;
        indices[index+5]=  x  +(y+1)*n;
        }

float* data=calloc(heightmap->num_vertices,sizeof(float));

//Generate texture
glGenTextures(1,&(heightmap->texture));
glBindTexture(GL_TEXTURE_2D,heightmap->texture);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
glTexImage2D(GL_TEXTURE_2D,0,GL_R32F,n,n,0,GL_RED,GL_FLOAT,data);

//Send buffer data to graphics card
glBindBuffer(GL_ARRAY_BUFFER,heightmap->vbo);
glBufferData(GL_ARRAY_BUFFER,2*heightmap->num_vertices*sizeof(float),vertices,GL_STATIC_DRAW);
glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,2*sizeof(float),(void*)0);
glEnableVertexAttribArray(0);

glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,heightmap->ibo);
glBufferData(GL_ELEMENT_ARRAY_BUFFER,heightmap->num_indices*sizeof(GLuint),indices,GL_DYNAMIC_DRAW);
glBindVertexArray(0);



//Client side memory is no longer needed
free(vertices);
free(indices);
free(data);
}

void heightmap_update(heightmap_t* heightmap,float* data)
{
glBindTexture(GL_TEXTURE_2D,heightmap->texture);
glTexImage2D(GL_TEXTURE_2D,0,GL_R32F,heightmap->n,heightmap->n,0,GL_RED,GL_FLOAT,data);
}


/*
void InitialiseRenderSystem()
{
CreateShaders();
InitBuffers();
InitHeightmaps();
}

void InitHeightmaps()
{
}
void UpdateHeightmap(float* texData)
{
glTexSubImage2D(GL_TEXTURE_2D,0,0,0,DOMAIN_POINTS,DOMAIN_POINTS,GL_LUMINANCE,GL_FLOAT,texData);
}
*/

void heightmap_render(heightmap_t* heightmap,const float* projection)
{
glUseProgram(resources.heightmap_shader.program);
glBindVertexArray(heightmap->vao);

//Bind textures
glActiveTexture(GL_TEXTURE0);
glBindTexture(GL_TEXTURE_2D,heightmap->texture);

//Assign uniforms
glUniformMatrix4fv(resources.heightmap_shader.mvp_loc,1,GL_FALSE,projection);
glUniform1f(resources.heightmap_shader.delta_x_loc,heightmap->delta_x);
glUniform1f(resources.heightmap_shader.map_size_loc,heightmap->size);
glUniform1i(resources.heightmap_shader.heightmap_loc,0);
glUniform3f(resources.heightmap_shader.offset_loc,heightmap->x_offset,heightmap->y_offset,heightmap->z_offset);
glUniform3f(resources.heightmap_shader.mat_color_loc,heightmap->color[0],heightmap->color[1],heightmap->color[2]);

//Render patch
glDrawElements(GL_TRIANGLES,heightmap->num_indices,GL_UNSIGNED_INT,0);
glBindVertexArray(0);
}
