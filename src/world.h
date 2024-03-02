#pragma once

#include <unordered_map>
#include <glm/glm.hpp>

#include "chunk.h"
#include "vechasher.h"

class World
{
public:
	explicit World(unsigned int chunkWidth, unsigned int chunkDepth)
		: m_chunkWidth(chunkWidth), m_chunkDepth(chunkDepth) 
	{
		m_chunks.emplace(glm::ivec2(0), glm::ivec2(chunkWidth, chunkDepth)); // Default chunk for testing 
	}

	void UpdateBlock(glm::ivec3 coordinate, bool set)
	{
		glm::ivec2 chunkPos = glm::ivec2(coordinate.x % m_chunkWidth, coordinate.z % m_chunkDepth);
		glm::ivec3 inChunkPos = glm::ivec3(coordinate.x / m_chunkWidth, coordinate.y, coordinate.z / m_chunkDepth);

		auto it = m_chunks.find(chunkPos);
		if (it == m_chunks.end())
		{
			std::cout << "Failed to find chunk at " << chunkPos.x << " : " << chunkPos.y << '\n';
		}
		else
		{
			Chunk& chunk = it->second;
			chunk.Update(inChunkPos, set);
		}
	}

private:
	unsigned int m_chunkWidth;
	unsigned int m_chunkDepth;
	std::unordered_map<glm::ivec2, Chunk, VecHasher<int, 2>> m_chunks;
};