#pragma once

#include <iostream>
#include <unordered_set>
#include <unordered_set>
#include <glm/glm.hpp>

#include "chunk.h"
#include "vechasher.h"
#include "blockprovider.h"

class World : public IBlockProvider
{
public:
	explicit World(int chunkWidth, int chunkDepth)
		: m_chunkWidth(chunkWidth), m_chunkDepth(chunkDepth)
	{
		m_chunks.emplace(glm::ivec2(0), glm::ivec2(chunkWidth, chunkDepth)); // Default chunk for testing 
	}

	void UpdateVoxel(glm::ivec3 coordinate, bool set)
	{
		glm::ivec2 chunkPos = glm::ivec2(coordinate.x / m_chunkWidth, coordinate.z / m_chunkDepth);
		glm::ivec3 inChunkPos = glm::ivec3(coordinate.x % m_chunkWidth, coordinate.y, coordinate.z % m_chunkDepth);

		auto it = m_chunks.find(chunkPos);
		if (it == m_chunks.end())
		{
			std::cout << "Failed to find chunk at " << chunkPos.x << " : " << chunkPos.y << '\n';
		}
		else
		{
			Chunk& chunk = it->second;
			chunk.UpdateVoxel(inChunkPos, set);
			m_dirtyChunks.insert(chunkPos);
		}
	}

	virtual bool GetVoxel(glm::ivec3 coordinate) const override
	{
		glm::ivec2 chunkPos = glm::ivec2(coordinate.x / m_chunkWidth, coordinate.z / m_chunkDepth);
		auto it = m_chunks.find(chunkPos);
		if (it == m_chunks.end())
		{
			std::cout << "Failed to find chunk at " << chunkPos.x << " : " << chunkPos.y << '\n';
			return false;
		}

		const Chunk& chunk = it->second;
		glm::ivec3 inChunkPos = glm::ivec3(coordinate.x % m_chunkWidth, coordinate.y, coordinate.z % m_chunkDepth);
		return chunk.GetVoxel(inChunkPos);
	}

	void ApplyChanges()
	{
		for (auto& chunkPos : m_dirtyChunks)
			m_chunks[chunkPos].GenerateMesh(*this);

		m_dirtyChunks.clear();
	}

	void Draw() const
	{
		for (const auto& chunk : m_chunks)
			chunk.second.Draw();
	}

private:
	int m_chunkWidth;
	int m_chunkDepth;
	std::unordered_map<glm::ivec2, Chunk, VecHasher<int, 2>> m_chunks;
	std::unordered_set<glm::ivec2, VecHasher<int, 2>> m_dirtyChunks;
};