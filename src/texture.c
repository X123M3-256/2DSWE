#include<stdlib.h>
#include<png.h>
#include "texture.h"

int load_png(const char* filename,int* w,int* h,char** data)
{
FILE *fp = fopen(filename, "rb");
	if(!fp)
	{
	printf("Failed loading file %s\n",filename);
	return -1;
	}
png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if(!png)
	{
	printf("Failed to create read struct\n");
	fclose(fp);
	return -1;
	}
png_infop info = png_create_info_struct(png);
	if(!info)
	{
	printf("Failed to create info struct\n");
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

*w=width;
*h=height;
*data=pixels;
}

int texture_load(const char* filename)
{
int width,height;
char* pixels;
	if(load_png(filename,&width,&height,&pixels)==-1)return -1;

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

int texture_load_cubemap(const char* filename)
{
int width,height;
char* pixels;
	if(load_png(filename,&width,&height,&pixels)==-1)return -1;
	if(height!=6*width)return -1;

int texture;
glGenTextures(1,&texture);
glBindTexture(GL_TEXTURE_CUBE_MAP,texture);
glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

	for(int i=0;i<6;i++)
	{
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i,0,GL_RGB,width,width,0,GL_RGB,GL_UNSIGNED_BYTE,pixels+3*i*width*width);
	}
free(pixels);	
return texture;
}
