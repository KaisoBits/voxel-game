#version 460

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

uniform mat4 model = mat4(1);
uniform mat4 view = mat4(1);
uniform mat4 perspective = mat4(1);

uniform vec3 chunkPos;

flat out int darken;
out vec2 textureUv;

void main()
{
	darken = abs(dot(normal, vec3(0, 0, -1))) > 0.1 ? 1 : 0;
	textureUv = uv;
	gl_Position = perspective * view * model * vec4(position + chunkPos, 1.0);
}