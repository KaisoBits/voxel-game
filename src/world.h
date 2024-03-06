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
	}

	void UpdateVoxel(glm::ivec3 coordinate, glm::ivec2 textureCoordinate)
	{
		glm::ivec3 chunkPos = coordinate / m_chunkDimensions;

		auto it = m_chunks.find(chunkPos);
		if (it == m_chunks.end())
		{
			m_chunks.emplace(chunkPos, Chunk(m_chunkDimensions, chunkPos));
			it = m_chunks.find(chunkPos);
		}

		Chunk& chunk = it->second;
		glm::ivec3 inChunkPos = coordinate % m_chunkDimensions;

		if (chunk.UpdateVoxel(inChunkPos, textureCoordinate))
		{
			m_dirtyChunks.insert(chunkPos);

			// TODO: Optimize for non edge voxels
			if (m_chunks.contains(chunkPos + glm::ivec3(0, 0, 1)))
				m_dirtyChunks.insert(chunkPos + glm::ivec3(0, 0, 1));
			if (m_chunks.contains(chunkPos + glm::ivec3(0, 0, -1)))
				m_dirtyChunks.insert(chunkPos + glm::ivec3(0, 0, -1));
			if (m_chunks.contains(chunkPos + glm::ivec3(0, 1, 0)))
				m_dirtyChunks.insert(chunkPos + glm::ivec3(0, 1, 0));
			if (m_chunks.contains(chunkPos + glm::ivec3(0, -1, 0)))
				m_dirtyChunks.insert(chunkPos + glm::ivec3(0, -1, 0));
			if (m_chunks.contains(chunkPos + glm::ivec3(1, 0, 0)))
				m_dirtyChunks.insert(chunkPos + glm::ivec3(1, 0, 0));
			if (m_chunks.contains(chunkPos + glm::ivec3(-1, 0, 0)))
				m_dirtyChunks.insert(chunkPos + glm::ivec3(-1, 0, 0));
		}
	}

	virtual glm::ivec2 GetVoxel(glm::ivec3 coordinate) const override
	{
		glm::ivec3 chunkPos = coordinate / m_chunkDimensions;
		auto it = m_chunks.find(chunkPos);
		if (it == m_chunks.end())
			return glm::ivec2(-1);

		const Chunk& chunk = it->second;
		glm::ivec3 inChunkPos = coordinate % m_chunkDimensions;
		return chunk.GetVoxel(inChunkPos);
	}

	void ApplyChanges()
	{
		for (auto& chunkPos : m_dirtyChunks)
			m_chunks.at(chunkPos).GenerateMesh(*this);

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