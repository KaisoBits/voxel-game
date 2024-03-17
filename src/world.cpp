#include <unordered_set>
#include <unordered_map>
#include <glm/glm.hpp>

#include "chunk.h"
#include "shader.h"
#include "vechasher.h"
#include "blockprovider.h"

#include "world.h"

World::World(glm::ivec3 chunkDimensions)
	: m_chunkDimensions(chunkDimensions)
{
}

void World::UpdateVoxel(const glm::ivec3& coordinate, const glm::ivec2& textureCoordinate)
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

		if (inChunkPos.x % m_chunkDimensions.x == 0 && m_chunks.contains(chunkPos + glm::ivec3(-1, 0, 0)))
			m_dirtyChunks.insert(chunkPos + glm::ivec3(-1, 0, 0));
		else if (inChunkPos.x % m_chunkDimensions.x == m_chunkDimensions.x - 1 && m_chunks.contains(chunkPos + glm::ivec3(1, 0, 0)))
			m_dirtyChunks.insert(chunkPos + glm::ivec3(1, 0, 0));

		if (inChunkPos.y % m_chunkDimensions.y == 0 && m_chunks.contains(chunkPos + glm::ivec3(0, -1, 0)))
			m_dirtyChunks.insert(chunkPos + glm::ivec3(0, -1, 0));
		else if (inChunkPos.y % m_chunkDimensions.y == m_chunkDimensions.y - 1 && m_chunks.contains(chunkPos + glm::ivec3(0, 1, 0)))
			m_dirtyChunks.insert(chunkPos + glm::ivec3(0, 1, 0));

		if (inChunkPos.z % m_chunkDimensions.z == 0 && m_chunks.contains(chunkPos + glm::ivec3(0, 0, -1)))
			m_dirtyChunks.insert(chunkPos + glm::ivec3(0, 0, -1));
		else if (inChunkPos.z % m_chunkDimensions.z == m_chunkDimensions.z - 1 && m_chunks.contains(chunkPos + glm::ivec3(0, 0, 1)))
			m_dirtyChunks.insert(chunkPos + glm::ivec3(0, 0, 1));
	}
}

glm::ivec2 World::GetVoxel(const glm::ivec3& coordinate) const
{
	glm::ivec3 chunkPos = coordinate / m_chunkDimensions;
	auto it = m_chunks.find(chunkPos);
	if (it == m_chunks.end())
		return glm::ivec2(-1);

	const Chunk& chunk = it->second;
	glm::ivec3 inChunkPos = coordinate % m_chunkDimensions;
	return chunk.GetVoxel(inChunkPos);
}

void World::ApplyChanges()
{
	for (auto& chunkPos : m_dirtyChunks)
	{
		bool notEmpty = m_chunks.at(chunkPos).GenerateMesh(*this);
		if (!notEmpty)
		{
			// m_dirtyChunks can contain non existing chunks
			// because of border-updates propagating to surrounding chunks
			// but it's fine to try to erease it anyway
			m_chunks.erase(chunkPos);
		}
	}

	m_dirtyChunks.clear();
}

void World::Draw(Shader& shader) const
{
	for (const auto& [_, chunk] : m_chunks)
		chunk.Draw(shader);
}