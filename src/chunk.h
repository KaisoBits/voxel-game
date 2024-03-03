#pragma once

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <unordered_map>
#include <vector>

#include "vechasher.h"
#include "blockprovider.h"
#include "blockdata.h"

class Chunk
{
public:
	explicit Chunk() {}
	explicit Chunk(glm::ivec2 dimensions) : m_dimensions(dimensions)
	{
		glGenVertexArrays(1, &m_meshVao);
		glBindVertexArray(m_meshVao);

		glGenBuffers(1, &m_meshVbo);
		glBindBuffer(GL_ARRAY_BUFFER, m_meshVbo);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, nullptr);
		glEnableVertexAttribArray(0);
		//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, reinterpret_cast<void*>(sizeof(float) * 3));
		//glEnableVertexAttribArray(1);
	}

	void UpdateVoxel(glm::ivec3 coordinate, bool set)
	{
		if (!set)
		{
			auto it = m_blocks.find(coordinate);
			if (it != m_blocks.end())
				m_blocks.erase(it);
		}
		else
		{
			m_blocks[coordinate] = {};
		}
	}

	bool GetVoxel(glm::ivec3 coordinate) const
	{
		auto it = m_blocks.find(coordinate);
		return it != m_blocks.end() && it->second;
	}

	void Draw() const
	{
		glBindVertexArray(m_meshVao);
		glDrawArrays(GL_TRIANGLES, 0, m_verticesCount);
	}

	void GenerateMesh(const IBlockProvider& blockProvider)
	{
		unsigned int vertCount = 0;

		glBindVertexArray(m_meshVao);
		glBindBuffer(GL_ARRAY_BUFFER, m_meshVbo);

		std::vector<float> vertexData;

		for (auto& pair : m_blocks)
		{
			glm::ivec3 globalPos = pair.first + glm::ivec3(m_position.x, 0, m_position.y);
			if (!blockProvider.GetVoxel(globalPos + glm::ivec3(0, 0, -1)))
			{
				vertexData.insert(vertexData.end(), std::begin(FRONT_FACE), std::end(FRONT_FACE));
				vertCount += 6;
			}
			if (!blockProvider.GetVoxel(globalPos + glm::ivec3(0, 0, 1)))
			{
				vertexData.insert(vertexData.end(), std::begin(BACK_FACE), std::end(BACK_FACE));
				vertCount += 6;
			}
			if (!blockProvider.GetVoxel(globalPos + glm::ivec3(0, 1, 0)))
			{
				vertexData.insert(vertexData.end(), std::begin(TOP_FACE), std::end(TOP_FACE));
				vertCount += 6;
			}
			if (!blockProvider.GetVoxel(globalPos + glm::ivec3(0, -1, 0)))
			{
				vertexData.insert(vertexData.end(), std::begin(BOTTOM_FACE), std::end(BOTTOM_FACE));
				vertCount += 6;
			}
			if (!blockProvider.GetVoxel(globalPos + glm::ivec3(1, 0, 0)))
			{
				vertexData.insert(vertexData.end(), std::begin(RIGHT_FACE), std::end(RIGHT_FACE));
				vertCount += 6;
			}
			if (!blockProvider.GetVoxel(globalPos + glm::ivec3(-1, 0, 0)))
			{
				vertexData.insert(vertexData.end(), std::begin(LEFT_FACE), std::end(LEFT_FACE));
				vertCount += 6;
			}
		}

		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertexData.size(), vertexData.data(), GL_DYNAMIC_DRAW);

		m_verticesCount = vertCount;
	}

private:
	unsigned int m_meshVao = 0;
	unsigned int m_verticesCount = 0;
	unsigned int m_meshVbo = 0;

	std::unordered_map<glm::ivec3, bool, VecHasher<int, 3>> m_blocks;
	glm::ivec2 m_position{};
	glm::ivec2 m_dimensions{};
};