#pragma once

#include <glm/vec3.hpp>

template <typename T, int count>
struct VecHasher
{
	std::size_t operator()(const glm::vec<count, T, glm::defaultp>& k) const
	{
		std::hash<int> hash = std::hash<int>();
		std::size_t resultHash = 0;
		for (size_t i = 0; i < count; i++)
		{
			resultHash ^= hash(k[i]);
		}

		return resultHash;
	}
};
