#pragma once

#include <glm/glm.hpp>

class IBlockProvider
{
public:
	virtual ~IBlockProvider() = default;
	virtual bool GetVoxel(glm::ivec3 coordinate) const = 0;
};