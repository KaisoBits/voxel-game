#include "world.h"

World::World(glm::ivec3 chunkDimensions)
	: m_chunkDimensions(chunkDimensions)
{
}

void World::UpdateVoxel(glm::ivec3 coordinate, glm::ivec2 textureCoordinate)
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

glm::ivec2 World::GetVoxel(glm::ivec3 coordinate) const
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
		m_chunks.at(chunkPos).GenerateMesh(*this);

	m_dirtyChunks.clear();
}

void World::Draw(Shader& shader) const
{
	for (const auto& [_, chunk] : m_chunks)
		chunk.Draw(shader);
}