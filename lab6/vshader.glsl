#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

out vec3 Normal;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float t;

void main()
{
    vec3 position_d = position;
    vec3 normal_d = normalize(mat3(transpose(inverse(projection * view * model))) * normal);

    position_d.x = position_d.x*cos(t);
    position_d.y = position_d.y*sin(t+position_d.x);
    vec3 position_dx = position_d - position;
    normal_d = normalize(normal_d + position_dx);

    gl_Position = projection * view * model * vec4(position_d, 1.0f);
    FragPos = vec3(model * vec4(position_d, 1.0f));
    Normal = mat3(transpose(inverse(model))) * normal_d;
}