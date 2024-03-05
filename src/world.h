#pragma once

#include <iostream>
#include <unordered_set>
#include <unordered_set>
#include <glm/glm.hpp>

#include "chunk.h"
#include "shader.h"
#include "vechasher.h"
#include "blockprovider.h"

class World : public IBlockProvider
{
public:
	explicit World(glm::ivec3 chunkDimensions)
		: m_chunkDimensions(chunkDimensions)
	{
		m_chunks.emplace(glm::ivec3(0), Chunk(chunkDimensions)); // Default chunk for testing
	}

	void UpdateVoxel(glm::ivec3 coordinate, bool set)
	{
		glm::ivec3 chunkPos = coordinate / m_chunkDimensions;

		auto it = m_chunks.find(chunkPos);
		if (it == m_chunks.end())
		{
			std::cout << "Failed to find chunk at " << chunkPos.x << " : " << chunkPos.y << '\n';
		}
		else
		{
			Chunk& chunk = it->second;
			glm::ivec3 inChunkPos = coordinate % m_chunkDimensions;
			chunk.UpdateVoxel(inChunkPos, set);
			m_dirtyChunks.insert(chunkPos);
		}
	}

	virtual bool GetVoxel(glm::ivec3 coordinate) const override
	{
		glm::ivec3 chunkPos = coordinate / m_chunkDimensions;
		auto it = m_chunks.find(chunkPos);
		if (it == m_chunks.end())
			return false;

		const Chunk& chunk = it->second;
		glm::ivec3 inChunkPos = coordinate % m_chunkDimensions;
		return chunk.GetVoxel(inChunkPos);
	}

	void ApplyChanges()
	{
		for (auto& chunkPos : m_dirtyChunks)
			m_chunks[chunkPos].GenerateMesh(*this);

		m_dirtyChunks.clear();
	}

	void Draw(Shader& shader) const
	{
		for (const auto& [_, chunk] : m_chunks)
			chunk.Draw(shader);
	}

private:
	glm::ivec3 m_chunkDimensions;
	std::unordered_map<glm::ivec3, Chunk, VecHasher<int, 3>> m_chunks;
	std::unordered_set<glm::ivec3, VecHasher<int, 3>> m_dirtyChunks;
};