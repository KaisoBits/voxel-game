#pragma once

#include <glm/glm.hpp>

template <typename T, glm::length_t count>
struct VecHasher
{
	std::size_t operator()(const glm::vec<count, T, glm::defaultp>& k) const
	{
		std::hash<glm::length_t> hash = std::hash<glm::length_t>();
		std::size_t resultHash = 0;
		for (glm::length_t i = 0; i < count; i++)
		{
			resultHash ^= hash(k[i]) << i * 5;
		}

		return resultHash;
	}
};
