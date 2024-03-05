#pragma once

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <unordered_map>
#include <vector>
#include <chrono>
#include <iostream>

#include "vechasher.h"
#include "shader.h"
#include "blockprovider.h"
#include "blockdata.h"

class Chunk
{
public:
	explicit Chunk() {}
	explicit Chunk(glm::ivec3 dimensions, glm::ivec3 position)
		: m_dimensions(dimensions), m_position(position)
	{
		glGenVertexArrays(1, &m_meshVao);
		glBindVertexArray(m_meshVao);

		glGenBuffers(1, &m_meshVbo);
		glBindBuffer(GL_ARRAY_BUFFER, m_meshVbo);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, nullptr);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, reinterpret_cast<void*>(sizeof(float) * 3));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, reinterpret_cast<void*>(sizeof(float) * 6));
		glEnableVertexAttribArray(2);
	}

	bool UpdateVoxel(glm::ivec3 coordinate, bool set)
	{
		if (!set)
		{
			auto it = m_blocks.find(coordinate);
			if (it != m_blocks.end())
			{
				m_blocks.erase(it);
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			bool result = m_blocks[coordinate];
			if (result)
				return false;

			m_blocks[coordinate] = true;
			return true;
		}
	}

	bool GetVoxel(glm::ivec3 coordinate) const
	{
		auto it = m_blocks.find(coordinate);
		return it != m_blocks.end() && it->second;
	}

	void Draw(Shader& shader) const
	{
		glm::ivec3 globalPos = m_position * m_dimensions;
		shader.SetVec3("chunkPos", globalPos);

		glBindVertexArray(m_meshVao);
		glDrawArrays(GL_TRIANGLES, 0, m_verticesCount);
	}

	void GenerateMesh(const IBlockProvider& blockProvider)
	{
		using namespace std::chrono;
		using namespace std::chrono_literals;

		unsigned int vertCount = 0;

		glBindVertexArray(m_meshVao);
		glBindBuffer(GL_ARRAY_BUFFER, m_meshVbo);

		std::vector<float> vertexData;

		auto generationStart = high_resolution_clock::now();

		for (auto& pair : m_blocks)
		{
			glm::ivec3 relativePos = pair.first;
			glm::ivec3 globalPos = m_position * m_dimensions + relativePos;
			if (!blockProvider.GetVoxel(globalPos + glm::ivec3(0, 0, 1)))
				PushFace(vertexData, FRONT_FACE, relativePos);
			if (!blockProvider.GetVoxel(globalPos + glm::ivec3(0, 0, -1)))
				PushFace(vertexData, BACK_FACE, relativePos);
			if (!blockProvider.GetVoxel(globalPos + glm::ivec3(0, 1, 0)))
				PushFace(vertexData, TOP_FACE, relativePos);
			if (!blockProvider.GetVoxel(globalPos + glm::ivec3(0, -1, 0)))
				PushFace(vertexData, BOTTOM_FACE, relativePos);
			if (!blockProvider.GetVoxel(globalPos + glm::ivec3(1, 0, 0)))
				PushFace(vertexData, RIGHT_FACE, relativePos);
			if (!blockProvider.GetVoxel(globalPos + glm::ivec3(-1, 0, 0)))
				PushFace(vertexData, LEFT_FACE, relativePos);
		}
		auto generationEnd = duration_cast<microseconds>(high_resolution_clock::now() - generationStart);

		auto bufferStart = high_resolution_clock::now();
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertexData.size(), vertexData.data(), GL_DYNAMIC_DRAW);
		auto bufferEnd = duration_cast<microseconds>(high_resolution_clock::now() - bufferStart);

		m_verticesCount = vertexData.size();

		// std::cout << "Generation: " << generationEnd << ". Buffer:" << bufferEnd << '\n';
	}

private:
	unsigned int m_meshVao = 0;
	unsigned int m_verticesCount = 0;
	unsigned int m_meshVbo = 0;

	std::unordered_map<glm::ivec3, bool, VecHasher<int, 3>> m_blocks;
	glm::ivec3 m_position{};
	glm::ivec3 m_dimensions{};

	void PushFace(std::vector<float>& result, const float data[], glm::vec3 relativePos)
	{
		constexpr size_t vertLen = std::size(FRONT_FACE);

		for (size_t i = 0; i < vertLen; i += 8)
		{
			// positions
			result.push_back(data[i + 0] * 1.02f + relativePos.x);
			result.push_back(data[i + 1] * 1.02f + relativePos.y);
			result.push_back(data[i + 2] * 1.02f + relativePos.z);

			// normals
			result.push_back(data[i + 3]);
			result.push_back(data[i + 4]);
			result.push_back(data[i + 5]);

			// uv
			constexpr float tileX = 64.0f;
			constexpr float tileY = 32.0f;

			float x = 1 / tileX;
			float y = 1 / tileY;
			result.push_back(data[i + 6] / tileX + x * 4);
			result.push_back(data[i + 7] / tileY + y * 7);
		}
	}
};