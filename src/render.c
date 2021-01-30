#include<stdlib.h>
#include<stdio.h>
#include<math.h>
#include<limits.h>
#include<png.h>
#include "render.h"

struct
{
	struct
	{
	GLuint program;
	GLuint mvp_loc;
	GLuint heightmap_loc;
	GLuint texture_loc;
	GLuint delta_x_loc;
	GLuint map_size_loc;
	GLuint offset_loc;
	}
	heightmap_shader;
	
	struct
	{
	GLuint program;
	GLuint mvp_loc;
	GLuint heightmap_loc;
	GLuint texture_loc;
	GLuint delta_x_loc;
	GLuint map_size_loc;
	GLuint offset_loc;
	GLuint u_loc;
	}
	water_shader;
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
resources.heightmap_shader.texture_loc=glGetUniformLocation(resources.heightmap_shader.program,"mat_texture");
resources.heightmap_shader.delta_x_loc=glGetUniformLocation(resources.heightmap_shader.program,"delta_x");
resources.heightmap_shader.map_size_loc=glGetUniformLocation(resources.heightmap_shader.program,"map_size");
resources.heightmap_shader.offset_loc=glGetUniformLocation(resources.heightmap_shader.program,"offset");

resources.water_shader.program=shader_build("shaders/vertex_water.glsl","shaders/fragment_water.glsl");
resources.water_shader.mvp_loc=glGetUniformLocation(resources.water_shader.program,"mvp");
resources.water_shader.heightmap_loc=glGetUniformLocation(resources.water_shader.program,"heightmap_loc");
resources.water_shader.texture_loc=glGetUniformLocation(resources.water_shader.program,"mat_texture");
resources.water_shader.delta_x_loc=glGetUniformLocation(resources.water_shader.program,"delta_x");
resources.water_shader.map_size_loc=glGetUniformLocation(resources.water_shader.program,"map_size");
resources.water_shader.offset_loc=glGetUniformLocation(resources.water_shader.program,"offset");
resources.water_shader.u_loc=glGetUniformLocation(resources.water_shader.program,"u");
}

int texture_load(const char* filename)
{
FILE *fp = fopen(filename, "rb");
	if(!fp)
	{
	return -1;
	}
png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if(!png)
	{
	fclose(fp);
	return -1;
	}
png_infop info = png_create_info_struct(png);
	if(!info)
	{
	fclose(fp);
	return -1;
	}	
	if(setjmp(png_jmpbuf(png))) abort();//TODO Not sure what this does but I don't think it's what I want

png_init_io(png,fp);
png_read_info(png,info);

int width=png_get_image_width(png,info);
int height=png_get_image_height(png,info);

png_byte color_type=png_get_color_type(png,info);
png_byte bit_depth=png_get_bit_depth(png,info);
	if(bit_depth==16)png_set_strip_16(png);
	if(color_type==PNG_COLOR_TYPE_PALETTE)png_set_palette_to_rgb(png);

// PNG_COLOR_TYPE_GRAY_ALPHA is always 8 or 16bit depth.
	if(color_type==PNG_COLOR_TYPE_GRAY&&bit_depth<8)png_set_expand_gray_1_2_4_to_8(png);
	if(png_get_valid(png, info, PNG_INFO_tRNS))png_set_tRNS_to_alpha(png);

// These color_type don't have an alpha channel then fill it with 0xff
	if(color_type==PNG_COLOR_TYPE_RGB||color_type==PNG_COLOR_TYPE_GRAY||color_type==PNG_COLOR_TYPE_PALETTE)png_set_filler(png, 0xFF, PNG_FILLER_AFTER);
	if(color_type == PNG_COLOR_TYPE_GRAY||color_type == PNG_COLOR_TYPE_GRAY_ALPHA)png_set_gray_to_rgb(png);

png_read_update_info(png, info);

png_bytep* row_pointers=malloc(sizeof(png_bytep)*height);
	for(int y=0;y<height;y++)
	{
	row_pointers[y]=malloc(sizeof(png_byte)*png_get_rowbytes(png,info));
	}

png_read_image(png,row_pointers);

unsigned char* pixels=malloc(3*width*height);

	for(int y=0;y<height;y++)
	for(int x=0;x<width;x++)
	for(int i=0;i<3;i++)
	{
	pixels[3*(x+y*width)+i]=row_pointers[y][4*x+i];
	}
	for(int y=0;y<height;y++)
	{
	free(row_pointers[y]);
	}
free(row_pointers);
fclose(fp);

int texture;
glGenTextures(1,&texture);
glBindTexture(GL_TEXTURE_2D,texture);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_REPEAT);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,width,height,0,GL_RGB,GL_UNSIGNED_BYTE,pixels);
free(pixels);
return texture;
}


void heightmap_init(heightmap_t* heightmap,int n,float size,float x_offset,float y_offset,float z_offset,const char* filename)
{
heightmap->n=n;
heightmap->size=size;
heightmap->x_offset=x_offset;
heightmap->y_offset=y_offset;
heightmap->z_offset=z_offset;
heightmap->delta_x=size/(n-1);
heightmap->texture=texture_load(filename);
	if(heightmap->texture<0)
	{
	printf("Failed to load texture %s\n",filename);
	}


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
glGenTextures(1,&(heightmap->height_texture));
glBindTexture(GL_TEXTURE_2D,heightmap->height_texture);
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
glBindTexture(GL_TEXTURE_2D,heightmap->height_texture);
glTexImage2D(GL_TEXTURE_2D,0,GL_R32F,heightmap->n,heightmap->n,0,GL_RED,GL_FLOAT,data);
}

void heightmap_render(heightmap_t* heightmap,const float* projection)
{
glUseProgram(resources.heightmap_shader.program);
glBindVertexArray(heightmap->vao);

//Bind textures
glActiveTexture(GL_TEXTURE0);
glBindTexture(GL_TEXTURE_2D,heightmap->height_texture);

glActiveTexture(GL_TEXTURE1);
glBindTexture(GL_TEXTURE_2D,heightmap->texture);

//Assign uniforms
glUniformMatrix4fv(resources.heightmap_shader.mvp_loc,1,GL_FALSE,projection);
glUniform1f(resources.heightmap_shader.delta_x_loc,heightmap->delta_x);
glUniform1f(resources.heightmap_shader.map_size_loc,heightmap->size);
glUniform1i(resources.heightmap_shader.heightmap_loc,0);
glUniform1i(resources.heightmap_shader.texture_loc,1);
glUniform3f(resources.heightmap_shader.offset_loc,heightmap->x_offset,heightmap->y_offset,heightmap->z_offset);

//Render patch
glDrawElements(GL_TRIANGLES,heightmap->num_indices,GL_UNSIGNED_INT,0);
glBindVertexArray(0);
}


#define TEXTURE_SIZE 5.0


void water_init(water_t* water,int n,float size,float x_offset,float y_offset,float z_offset,const char* filename)
{
water->n=n;
water->size=size;
water->x_offset=x_offset;
water->y_offset=y_offset;
water->z_offset=z_offset;
water->delta_x=size/(n-1);
water->u=0;
water->texture=texture_load(filename);
	if(water->texture<0)
	{
	printf("Failed to load texture %s\n",filename);
	}


glGenVertexArrays(1, &(water->vao));  
glBindVertexArray(water->vao);

glGenBuffers(1,&(water->vbo));
glGenBuffers(2,water->uvbo);
glGenBuffers(1,&(water->ibo));
water->num_vertices=(n*n);
water->num_indices=(6*(n-1)*(n-1));

float* vertices=malloc(2*water->num_vertices*sizeof(float));
water->uvs[0]=malloc(2*water->num_vertices*sizeof(float));
water->uvs[1]=malloc(2*water->num_vertices*sizeof(float));
GLuint* indices=malloc(water->num_indices*sizeof(GLuint));

//Generate triangles
        for(int y=0;y<n;y++)
        for(int x=0;x<n;x++)
        {
	int index=2*(x+y*n);
        vertices[index]=x*water->delta_x;
        vertices[index+1]=y*water->delta_x;
        water->uvs[0][index]=x*water->delta_x/TEXTURE_SIZE;
        water->uvs[0][index+1]=y*water->delta_x/TEXTURE_SIZE;
        water->uvs[1][index]=x*water->delta_x/TEXTURE_SIZE;
        water->uvs[1][index+1]=y*water->delta_x/TEXTURE_SIZE;
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

float* data=calloc(water->num_vertices,sizeof(float));

//Generate texture
glGenTextures(1,&(water->height_texture));
glBindTexture(GL_TEXTURE_2D,water->height_texture);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
glTexImage2D(GL_TEXTURE_2D,0,GL_R32F,n,n,0,GL_RED,GL_FLOAT,data);

//Send buffer data to graphics card
glBindBuffer(GL_ARRAY_BUFFER,water->vbo);
glBufferData(GL_ARRAY_BUFFER,2*water->num_vertices*sizeof(float),vertices,GL_STATIC_DRAW);
glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,2*sizeof(float),(void*)0);

glBindBuffer(GL_ARRAY_BUFFER,water->uvbo[0]);
glBufferData(GL_ARRAY_BUFFER,2*water->num_vertices*sizeof(float),water->uvs[0],GL_DYNAMIC_DRAW);
glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,2*sizeof(float),(void*)0);

glBindBuffer(GL_ARRAY_BUFFER,water->uvbo[1]);
glBufferData(GL_ARRAY_BUFFER,2*water->num_vertices*sizeof(float),water->uvs[1],GL_DYNAMIC_DRAW);
glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,2*sizeof(float),(void*)0);

glEnableVertexAttribArray(0);
glEnableVertexAttribArray(1);
glEnableVertexAttribArray(2);

glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,water->ibo);
glBufferData(GL_ELEMENT_ARRAY_BUFFER,water->num_indices*sizeof(GLuint),indices,GL_DYNAMIC_DRAW);
glBindVertexArray(0);



//Client side memory is no longer needed
free(vertices);
free(indices);
free(data);
}





void reset_uvs(water_t* water,float* uvs)
{
float u_offset=rand()/(float)RAND_MAX;
float v_offset=rand()/(float)RAND_MAX;
	for(int y=0;y<water->n;y++)
	for(int x=0;x<water->n;x++)
	{
	int index=2*(x+y*water->n);
	uvs[index]=x*water->delta_x/TEXTURE_SIZE+u_offset;
	uvs[index+1]=y*water->delta_x/TEXTURE_SIZE+v_offset;
	}
}

void step_uvs(water_t* water,float* uvs,float* velocity_x,float* velocity_y,float dt)
{
	for(uint32_t y=0;y<water->n;y++)
	for(uint32_t x=0;x<water->n;x++)
	{
	int index=2*(x+y*water->n);
	uvs[index]-=dt*velocity_x[x+y*water->n]/TEXTURE_SIZE;
	uvs[index+1]-=dt*velocity_y[x+y*water->n]/TEXTURE_SIZE;
	}

}

#define TEXTURE_CYCLE_TIME 1.5

void water_update(water_t* water,float* data,float* velocity_x,float* velocity_y,float dt)
{
float du=dt/TEXTURE_CYCLE_TIME;
	if(water->u+du>1.0)
	{
	reset_uvs(water,water->uvs[0]);
	water->u=water->u+du-1.0;
	step_uvs(water,water->uvs[0],velocity_x,velocity_y,TEXTURE_CYCLE_TIME*water->u);	
	}
	else
	{
	step_uvs(water,water->uvs[0],velocity_x,velocity_y,dt);	
	water->u+=du;
	}

	if(water->u<0.5&&water->u+du>=0.5)
	{
	reset_uvs(water,water->uvs[1]);
	step_uvs(water,water->uvs[1],velocity_x,velocity_y,TEXTURE_CYCLE_TIME*(water->u+du-0.5));	
	}
	else step_uvs(water,water->uvs[1],velocity_x,velocity_y,dt);	

glBindBuffer(GL_ARRAY_BUFFER,water->uvbo[0]);
glBufferData(GL_ARRAY_BUFFER,2*water->num_vertices*sizeof(float),water->uvs[0],GL_DYNAMIC_DRAW);
glBindBuffer(GL_ARRAY_BUFFER,water->uvbo[1]);
glBufferData(GL_ARRAY_BUFFER,2*water->num_vertices*sizeof(float),water->uvs[1],GL_DYNAMIC_DRAW);

glBindTexture(GL_TEXTURE_2D,water->height_texture);
glTexImage2D(GL_TEXTURE_2D,0,GL_R32F,water->n,water->n,0,GL_RED,GL_FLOAT,data);
}

void water_render(water_t* water,const float* projection)
{
glUseProgram(resources.water_shader.program);
glBindVertexArray(water->vao);

//Bind textures
glActiveTexture(GL_TEXTURE0);
glBindTexture(GL_TEXTURE_2D,water->height_texture);

glActiveTexture(GL_TEXTURE1);
glBindTexture(GL_TEXTURE_2D,water->texture);

//Assign uniforms
glUniformMatrix4fv(resources.water_shader.mvp_loc,1,GL_FALSE,projection);
glUniform1f(resources.water_shader.delta_x_loc,water->delta_x);
glUniform1f(resources.water_shader.map_size_loc,water->size);
glUniform1f(resources.water_shader.u_loc,water->u);
glUniform1i(resources.water_shader.heightmap_loc,0);
glUniform1i(resources.water_shader.texture_loc,1);
glUniform3f(resources.water_shader.offset_loc,water->x_offset,water->y_offset,water->z_offset);

//Render patch
glDrawElements(GL_TRIANGLES,water->num_indices,GL_UNSIGNED_INT,0);
glBindVertexArray(0);
}
