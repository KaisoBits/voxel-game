#pragma once

#include <glm/glm.hpp>
#include <unordered_map>

#include "vechasher.h"
#include "shader.h"
#include "blockprovider.h"

class Chunk
{
public:
	explicit Chunk(glm::ivec3 dimensions, glm::ivec3 position);

	bool UpdateVoxel(const glm::ivec3& coordinate, const glm::ivec2& textureCoordinate);
	glm::vec2 GetVoxel(const glm::ivec3& coordinate) const;

	void Draw(Shader& shader) const;

	bool GenerateMesh(const IBlockProvider& blockProvider);

private:
	unsigned int m_meshVao = 0;
	unsigned int m_verticesCount = 0;
	unsigned int m_meshVbo = 0;

	std::unordered_map<glm::ivec3, glm::ivec2, VecHasher<int, 3>> m_blocks;
	glm::ivec3 m_position{};
	glm::ivec3 m_dimensions{};

	void PushFace(std::vector<float>& result, const float data[], const glm::vec3& relativePos, const glm::ivec2& textureCoord);
};