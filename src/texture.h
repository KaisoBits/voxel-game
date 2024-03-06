#pragma once

#include <string>

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