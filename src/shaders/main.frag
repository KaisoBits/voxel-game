#version 460

out vec4 FragColor;

in float darken;
in vec2 textureUv;

uniform sampler2D blockTexture;

void main()
{
	vec4 color = texture(blockTexture, textureUv);
	if (color.a < 0.1)
		discard;

	FragColor = vec4(color.xyz * darken, 1.0);
}