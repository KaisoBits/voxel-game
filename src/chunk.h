#pragma once

#include <glm/glm.hpp>
#include <unordered_map>

#include "vechasher.h"

class Chunk
{
public:
	explicit Chunk(glm::ivec2 dimensions) : m_dimensions(dimensions) {}

	void Update(glm::ivec3 coordinate, bool set)
	{
		if (!set)
		{
			auto it = m_blocks.find(coordinate);
			if (it != m_blocks.end())
				m_blocks.erase(it);
		}
		else
		{
			m_blocks[coordinate] = {};
		}
	}

private:
	std::unordered_map<glm::ivec3, bool, VecHasher<int, 3>> m_blocks;
	glm::ivec2 m_position{};
	glm::ivec2 m_dimensions{};
};