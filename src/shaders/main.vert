#version 460

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

uniform mat4 view = mat4(1);
uniform mat4 perspective = mat4(1);

uniform vec3 chunkPos;

out float darken;
out vec2 textureUv;

void main()
{
	if (abs(dot(normal, vec3(0, 0, 1))) > 0.1)
		darken = 0.70;
	else if (dot(normal, vec3(0, -1, 0)) > 0.1)
		darken = 0.5;
	else if (abs(dot(normal, vec3(0, 1, 0))) > 0.1)
		darken = 1.0;
	else
		darken = 0.85;

	textureUv = uv;
	gl_Position = perspective * view * vec4(position + chunkPos, 1.0);
}