#pragma once

#include <unordered_set>
#include <unordered_map>
#include <glm/glm.hpp>

#include "chunk.h"
#include "shader.h"
#include "vechasher.h"
#include "blockprovider.h"

class World : public IBlockProvider
{
public:
	explicit World(glm::ivec3 chunkDimensions);

	void UpdateVoxel(const glm::ivec3& coordinate, const glm::ivec2& textureCoordinate);
	virtual glm::ivec2 GetVoxel(const glm::ivec3& coordinate) const override;
	void ApplyChanges();

	void Draw(Shader& shader) const;
private:
	glm::ivec3 m_chunkDimensions;
	std::unordered_map<glm::ivec3, Chunk, VecHasher<int, 3>> m_chunks;
	std::unordered_set<glm::ivec3, VecHasher<int, 3>> m_dirtyChunks;
};