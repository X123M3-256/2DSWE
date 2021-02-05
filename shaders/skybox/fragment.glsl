#version 410

in vec3 direction;

out vec4 fragColor;

uniform samplerCube cubemap;

void main()
{
fragColor=texture(cubemap,direction);
}
