#pragma once

#include <glm/glm.hpp>

class IBlockProvider
{
public:
	virtual ~IBlockProvider() = default;
	virtual glm::ivec2 GetVoxel(glm::ivec3 coordinate) const = 0;
};