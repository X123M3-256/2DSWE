#include "vectormath.h"
#include <math.h>


vector2_t vector2(float x,float y)
{
vector2_t result={x,y};
return result;
}
vector2_t vector2_add(vector2_t a,vector2_t b)
{
return vector2(a.x+b.x,a.y+b.y);
}
vector2_t vector2_sub(vector2_t a,vector2_t b)
{
return vector2(a.x-b.x,a.y-b.y);
}
vector2_t vector2_mult(vector2_t a,float b)
{
return vector2(a.x*b,a.y*b);
}
float vector2_norm(vector2_t a)
{
return sqrt(a.x*a.x+a.y*a.y);
}


vector3_t vector3(float x,float y,float z)
{
vector3_t result={x,y,z};
return result;
}
vector3_t vector3_from_scalar(float a)
{
return vector3(a,a,a);
}
vector3_t vector3_add(vector3_t a,vector3_t b)
{
return vector3(a.x+b.x,a.y+b.y,a.z+b.z);
}
vector3_t vector3_sub(vector3_t a,vector3_t b)
{
return vector3(a.x-b.x,a.y-b.y,a.z-b.z);
}
vector3_t vector3_scale(vector3_t a,float b)
{
return vector3(a.x*b,a.y*b,a.z*b);
}
float vector3_dot(vector3_t a,vector3_t b)
{
return a.x*b.x+a.y*b.y+a.z*b.z;
}
vector3_t vector3_cross(vector3_t a,vector3_t b)
{
return vector3(a.y*b.z-a.z*b.y,a.z*b.x-a.x*b.z,a.x*b.y-a.y*b.x);
}
float vector3_magnitude(vector3_t a)
{
return sqrt(vector3_dot(a,a));
}
vector3_t vector3_normalize(vector3_t a)
{
return vector3_scale(a,1.0/vector3_magnitude(a));
}

quaternion_t quaternion(float w,float i,float j,float k)
{
quaternion_t q={w,i,j,k};
return q;
}
quaternion_t quaternion_identity()
{
return quaternion(1,0,0,0);
}
quaternion_t quaternion_axis(vector3_t axis)
{
float angle=vector3_magnitude(axis);
	if(angle<0.000001)return quaternion(1.0,axis.x,axis.y,axis.z);
return quaternion_axis_angle(angle,vector3_scale(axis,1.0/angle));
}
quaternion_t quaternion_axis_angle(float angle,vector3_t axis)
{
float s=sin(0.5*angle);
return quaternion(cos(0.5*angle),s*axis.x,s*axis.y,s*axis.z);
}
quaternion_t quaternion_scale(quaternion_t q,float s)
{
return quaternion(s*q.w,s*q.i,s*q.j,s*q.k);
}
quaternion_t quaternion_conjugate(quaternion_t q)
{
return quaternion(q.w,-q.i,-q.j,-q.k);
}
quaternion_t quaternion_add(quaternion_t a,quaternion_t b)
{
return quaternion(a.w+b.w,a.i+b.i,a.j+b.j,a.k+b.k);
}
quaternion_t quaternion_mult(quaternion_t a,quaternion_t b)
{
return quaternion((a.w*b.w)-(a.i*b.i)-(a.j*b.j)-(a.k*b.k),(a.w*b.i)+(a.i*b.w)+(a.j*b.k)-(a.k*b.j),(a.w*b.j)-(a.i*b.k)+(a.j*b.w)+(a.k*b.i),(a.w*b.k)+(a.i*b.j)-(a.j*b.i)+(a.k*b.w));
}

quaternion_t quaternion_inverse(quaternion_t q)
{
return quaternion_scale(quaternion_conjugate(q),1.0/(q.w*q.w+q.i*q.i+q.j*q.j+q.k*q.k));
}

float quaternion_magnitude(quaternion_t q)
{
return sqrt(q.w*q.w+q.i*q.i+q.j*q.j+q.k*q.k);
}
quaternion_t quaternion_normalize(quaternion_t q)
{
float magnitude=quaternion_magnitude(q);
return quaternion_scale(q,1.0/magnitude);
}

vector3_t quaternion_vector(quaternion_t q,vector3_t vec)
{
quaternion_t qv=quaternion((q.i*vec.x)+(q.j*vec.y)+(q.k*vec.z),(q.w*vec.x)-(q.j*vec.z)+(q.k*vec.y),(q.w*vec.y)+(q.i*vec.z)-(q.k*vec.x),(q.w*vec.z)-(q.i*vec.y)+(q.j*vec.x));
return vector3((qv.w*q.i)+(qv.i*q.w)+(qv.j*q.k)-(qv.k*q.j),(qv.w*q.j)-(qv.i*q.k)+(qv.j*q.w)+(qv.k*q.i),(qv.w*q.k)+(qv.i*q.j)-(qv.j*q.i)+(qv.k*q.w));
}




matrix_t matrix(float a,float b,float c,float d,float e,float f,float g,float h,float i,float j,float k,float l,float m,float n,float o,float p)
{
matrix_t mat={{a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p}};
return mat;
}
matrix_t matrix_identity()
{
return matrix(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1);
}

/*
float matrix_determinant(matrix_t mat)
{
return MATRIX_INDEX(mat,0,0)*(MATRIX_INDEX(mat,1,1)*MATRIX_INDEX(mat,2,2)-MATRIX_INDEX(mat,1,2)*MATRIX_INDEX(mat,2,1))-
       MATRIX_INDEX(mat,0,1)*(MATRIX_INDEX(mat,1,0)*MATRIX_INDEX(mat,2,2)-MATRIX_INDEX(mat,1,2)*MATRIX_INDEX(mat,2,0))+
       MATRIX_INDEX(mat,0,2)*(MATRIX_INDEX(mat,1,0)*MATRIX_INDEX(mat,2,1)-MATRIX_INDEX(mat,1,1)*MATRIX_INDEX(mat,2,0));
}
matrix_t matrix_inverse(matrix_t mat)
{
float determinant=matrix_determinant(mat);
return matrix(
	(MATRIX_INDEX(mat,1,1)*MATRIX_INDEX(mat,2,2)-MATRIX_INDEX(mat,1,2)*MATRIX_INDEX(mat,2,1))/determinant,
	(MATRIX_INDEX(mat,0,2)*MATRIX_INDEX(mat,2,1)-MATRIX_INDEX(mat,2,2)*MATRIX_INDEX(mat,0,1))/determinant,
	(MATRIX_INDEX(mat,0,1)*MATRIX_INDEX(mat,1,2)-MATRIX_INDEX(mat,1,1)*MATRIX_INDEX(mat,0,2))/determinant,

	(MATRIX_INDEX(mat,1,2)*MATRIX_INDEX(mat,2,0)-MATRIX_INDEX(mat,2,2)*MATRIX_INDEX(mat,1,0))/determinant,
	(MATRIX_INDEX(mat,0,0)*MATRIX_INDEX(mat,2,2)-MATRIX_INDEX(mat,2,0)*MATRIX_INDEX(mat,0,2))/determinant,
	(MATRIX_INDEX(mat,0,2)*MATRIX_INDEX(mat,1,0)-MATRIX_INDEX(mat,1,2)*MATRIX_INDEX(mat,0,0))/determinant,

	(MATRIX_INDEX(mat,1,0)*MATRIX_INDEX(mat,2,1)-MATRIX_INDEX(mat,2,0)*MATRIX_INDEX(mat,1,1))/determinant,
	(MATRIX_INDEX(mat,0,1)*MATRIX_INDEX(mat,2,0)-MATRIX_INDEX(mat,2,1)*MATRIX_INDEX(mat,0,0))/determinant,
	(MATRIX_INDEX(mat,0,0)*MATRIX_INDEX(mat,1,1)-MATRIX_INDEX(mat,1,0)*MATRIX_INDEX(mat,0,1))/determinant
);
}
*/
matrix_t matrix_transpose(matrix_t mat)
{
return matrix(MATRIX_INDEX(mat,0,0),MATRIX_INDEX(mat,1,0),MATRIX_INDEX(mat,2,0),MATRIX_INDEX(mat,3,0),MATRIX_INDEX(mat,0,1),MATRIX_INDEX(mat,1,1),MATRIX_INDEX(mat,2,1),MATRIX_INDEX(mat,3,1),MATRIX_INDEX(mat,0,2),MATRIX_INDEX(mat,1,2),MATRIX_INDEX(mat,2,2),MATRIX_INDEX(mat,3,2),MATRIX_INDEX(mat,0,3),MATRIX_INDEX(mat,1,3),MATRIX_INDEX(mat,2,3),MATRIX_INDEX(mat,3,3));
}

matrix_t matrix_mult(matrix_t a,matrix_t b)
{
matrix_t result;
    for(int col=0;col<4;col++)
    for(int row=0;row<4;row++)
    {
    MATRIX_INDEX(result,row,col)=MATRIX_INDEX(a,row,0)*MATRIX_INDEX(b,0,col)+MATRIX_INDEX(a,row,1)*MATRIX_INDEX(b,1,col)+MATRIX_INDEX(a,row,2)*MATRIX_INDEX(b,2,col)+MATRIX_INDEX(a,row,3)*MATRIX_INDEX(b,3,col);
    }
return result;
}

vector3_t matrix_vector(matrix_t mat,vector3_t vector)
{
vector3_t result;
result.x=MATRIX_INDEX(mat,0,0)*vector.x+MATRIX_INDEX(mat,0,1)*vector.y+MATRIX_INDEX(mat,0,2)*vector.z;
result.y=MATRIX_INDEX(mat,1,0)*vector.x+MATRIX_INDEX(mat,1,1)*vector.y+MATRIX_INDEX(mat,1,2)*vector.z;
result.z=MATRIX_INDEX(mat,2,0)*vector.x+MATRIX_INDEX(mat,2,1)*vector.y+MATRIX_INDEX(mat,2,2)*vector.z;
return result;
}

matrix_t matrix_translate(vector3_t a)
{
return matrix(1,0,0,a.x,0,1,0,a.y,0,0,1,a.z,0,0,0,1);
}

matrix_t matrix_rotate_x(float theta)
{
return matrix(1,0,0,0, 0,cos(theta),-sin(theta),0, 0,sin(theta),cos(theta),0, 0,0,0,1);
}
matrix_t matrix_rotate_y(float theta)
{
return matrix(cos(theta),0,sin(theta),0,0,1,0,0,-sin(theta),0,cos(theta),0,0,0,0,1);
}
matrix_t matrix_rotate_z(float theta)
{
return matrix(cos(theta),-sin(theta),0,0,sin(theta),cos(theta),0,0,0,0,1,0,0,0,0,1);
}

matrix_t matrix_rotate(quaternion_t q)
{
return matrix(1-2*(q.j*q.j+q.k*q.k),2*(q.i*q.j+q.k*q.w),2*(q.i*q.k-q.j*q.w),0,
2*(q.i*q.j-q.k*q.w),1-2*(q.i*q.i+q.k*q.k),2*(q.j*q.k+q.i*q.w),0,
2*(q.i*q.k+q.j*q.w),2*(q.j*q.k-q.i*q.w),1-2*(q.i*q.i+q.j*q.j),0,
0,0,0,1);
}

