#version 410

layout(location=0) in vec2 position;

out vec3 normal;

uniform mat4 mvp;
uniform float map_size;
uniform float delta_x;
uniform vec3 offset;
uniform sampler2D heightmap;

void main()
{
//Transform the vertex into world space

vec2 uv=position/map_size;
float uv_delta_x=delta_x/map_size;
float height=texture(heightmap,uv).r;

float dx=(texture(heightmap,uv+vec2(uv_delta_x,0)).r-texture(heightmap,uv+vec2(-uv_delta_x,0)).r)/(2.0*delta_x);
float dy=(texture(heightmap,uv+vec2(0,uv_delta_x)).r-texture(heightmap,uv+vec2(0,-uv_delta_x)).r)/(2.0*delta_x);


vec4 worldSpacePosition=vec4(position+offset.xy,height+offset.z,1.0);

normal=normalize(vec3(-dx,-dy,1.0));

gl_Position=mvp*worldSpacePosition;
}
