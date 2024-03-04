#version 460

out vec4 FragColor;

flat in int darken;

void main()
{
	float mult = darken == 1 ? 0.7 : 1.0;
	FragColor = vec4(vec3(1.0, 1.0, 0.0) * mult, 1.0);
}