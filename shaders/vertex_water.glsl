#version 410

layout(location=0) in vec2 position;
layout(location=1) in vec2 uv_in;
layout(location=2) in vec2 uv2_in;

out vec3 normal;
out vec2 uv;
out vec2 uv2;

uniform mat4 mvp;
uniform float map_size;
uniform float delta_x;
uniform vec3 offset;
uniform sampler2D heightmap;

void main()
{
//Transform the vertex into world space


vec2 height_uv=position/map_size;
float uv_delta_x=delta_x/map_size;
float height=texture(heightmap,height_uv).r;

float dx=(texture(heightmap,height_uv+vec2(uv_delta_x,0)).r-texture(heightmap,height_uv+vec2(-uv_delta_x,0)).r)/(2.0*delta_x);
float dy=(texture(heightmap,height_uv+vec2(0,uv_delta_x)).r-texture(heightmap,height_uv+vec2(0,-uv_delta_x)).r)/(2.0*delta_x);


vec4 worldSpacePosition=vec4(position+offset.xy,height+offset.z,1.0);

normal=normalize(vec3(-dx,-dy,1.0));
uv=uv_in;
uv2=uv2_in;

gl_Position=mvp*worldSpacePosition;
}
