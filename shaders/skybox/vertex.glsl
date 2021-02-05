#version 410

layout(location=0) in vec2 position;

out vec3 direction;

uniform mat4 camera;
uniform float normalized_width;
uniform float normalized_height;

void main()
{
direction=mat3(camera)*vec3(-position.x*normalized_width,-position.y*normalized_height,1.0);
direction=direction.xzy;
direction.y=-direction.y;

gl_Position=vec4(position,1.0,1.0);
}
