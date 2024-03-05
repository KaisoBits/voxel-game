#version 460

out vec4 FragColor;

flat in int darken;
in vec2 textureUv;

uniform sampler2D blockTexture;

void main()
{
	float mult = darken == 1 ? 0.7 : 1.0;
	vec4 color = texture(blockTexture, textureUv);
	if (color.a < 0.1)
		discard;

	FragColor = vec4(color.xyz * mult, 1.0);
}