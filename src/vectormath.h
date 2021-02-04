#ifndef FIXED_H_INCLUDED
#define FIXED_H_INCLUDED
#include <stdint.h>

typedef struct
{
float x,y,z;
}vector3_t;

typedef struct
{
float x,y;
}vector2_t;

typedef struct
{
float entries[16];
}matrix_t;

typedef struct
{
float w,i,j,k;
}quaternion_t;

vector2_t vector2(float x,float y);
vector2_t vector2_add(vector2_t a,vector2_t b);
vector2_t vector2_sub(vector2_t a,vector2_t b);
vector2_t vector2_scale(vector2_t a,float b);
float vector2_norm(vector2_t);
float vector2_dot(vector2_t a,vector2_t b);

vector3_t vector3(float x,float y,float z);
vector3_t vector3_from_scalar(float a);
vector3_t vector3_add(vector3_t a,vector3_t b);
vector3_t vector3_sub(vector3_t a,vector3_t b);
vector3_t vector3_scale(vector3_t a,float b);
float vector3_norm(vector3_t a);
float vector3_dot(vector3_t a,vector3_t b);
vector3_t vector3_normalize(vector3_t a);
vector3_t vector3_cross(vector3_t a,vector3_t b);

quaternion_t quaternion(float w,float i,float j,float k);
quaternion_t quaternion_identity();
quaternion_t quaternion_axis(vector3_t axis);
quaternion_t quaternion_axis_angle(float angle,vector3_t axis);
quaternion_t quaternion_scale(quaternion_t a,float b);
quaternion_t quaternion_conjugate(quaternion_t q);
float quaternion_magnitude(quaternion_t q);
quaternion_t quaternion_normalize(quaternion_t q);
quaternion_t quaternion_add(quaternion_t a,quaternion_t b);
quaternion_t quaternion_mult(quaternion_t a,quaternion_t b);
quaternion_t quaternion_inverse(quaternion_t q);
vector3_t quaternion_vector(quaternion_t q,vector3_t vec);

#define MATRIX_INDEX(matrix,row,col) ((matrix).entries[4*(row)+(col)])
matrix_t matrix(float a,float b,float c,float d,float e,float f,float g,float h,float i,float j,float k,float l,float m,float n,float o,float p);
matrix_t matrix_identity();
float matrix_determinant(matrix_t matrix);
matrix_t matrix_inverse(matrix_t matrix);
matrix_t matrix_transpose(matrix_t matrix);
matrix_t matrix_mult(matrix_t a,matrix_t b);
vector3_t matrix_vector(matrix_t matrix,vector3_t vector);

matrix_t matrix_translate(vector3_t a);
matrix_t matrix_rotate_x(float angle);
matrix_t matrix_rotate_y(float angle);
matrix_t matrix_rotate_z(float angle);
matrix_t matrix_rotate(quaternion_t q);

#endif // FIXED_H_INCLUDED
