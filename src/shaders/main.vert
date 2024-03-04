#version 460

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

uniform mat4 model = mat4(1);
uniform mat4 view = mat4(1);
uniform mat4 perspective = mat4(1);

flat out int darken;

void main()
{
	darken = abs(dot(normal, vec3(0, 0, -1))) > 0.1 ? 1 : 0;
	gl_Position = perspective * view * model * vec4(position, 1.0);
}