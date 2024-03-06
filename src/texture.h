#pragma once

#include <stb/stb_image.h>

#include <string>
#include <iostream>
#include <glad/glad.h>

class Texture
{
public:
	static Texture Load(const std::string& path, bool flip);
	static Texture Empty() { return Texture(0); }

	void Use(unsigned int slot);

private:
	explicit Texture(unsigned int id) : m_id(id) {};

	unsigned int m_id;
};