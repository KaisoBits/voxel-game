#pragma once

#include <stb/stb_image.h>

#include <string>
#include <iostream>
#include <glad/glad.h>

class Texture
{
public:
	static Texture Load(const std::string& path, bool flip)
	{
		stbi_set_flip_vertically_on_load(flip);

		int width, height, channels;
		unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, STBI_default);
		if (!data)
		{
			std::cout << "Failed to load texture " << path << '\n';
			return Empty();
		}

		unsigned int textureId;
		glGenTextures(1, &textureId);
		glBindTexture(GL_TEXTURE_2D, textureId);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, channels == 4 ? GL_RGBA : GL_RGB, width, height, 0, channels == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		stbi_image_free(data);

		return Texture(textureId);
	}

	static Texture Empty() { return Texture(0); }

	void Use(unsigned int slot)
	{
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, m_id);
	}

private:
	explicit Texture(unsigned int id) : m_id(id) {};

	unsigned int m_id;
};