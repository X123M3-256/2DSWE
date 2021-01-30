#version 410

in vec3 normal;
in vec2 uv;

out vec4 fragColor;

uniform sampler2D mat_texture;

void main()
{
vec3 light_dir=normalize(vec3(-0.5,1,1));
vec3 view_dir = normalize(vec3(-1,-1,1));
vec3 reflect_dir=reflect(-light_dir, normal);  
float ambient=0.35;
float lambert=0.75*clamp(dot(normal,light_dir),0.0,1.0);
float specular=pow(max(dot(view_dir,reflect_dir),0.0),64);

vec3 color=texture(mat_texture,uv).xyz;
fragColor=vec4((ambient+lambert+specular)*color,1.0);
}
