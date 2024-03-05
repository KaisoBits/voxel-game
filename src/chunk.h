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
	explicit Chunk(glm::ivec2 dimensions) : m_dimensions(dimensions)
	{
		glGenVertexArrays(1, &m_meshVao);
		glBindVertexArray(m_meshVao);

		glGenBuffers(1, &m_meshVbo);
		glBindBuffer(GL_ARRAY_BUFFER, m_meshVbo);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, nullptr);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, reinterpret_cast<void*>(sizeof(float) * 3));
		glEnableVertexAttribArray(1);
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
			m_blocks[coordinate] = true;
		}
	}

	bool GetVoxel(glm::ivec3 coordinate) const
	{
		auto it = m_blocks.find(coordinate);
		return it != m_blocks.end() && it->second;
	}

	void Draw(Shader& shader) const
	{
		glm::ivec2 globalPos = m_position * m_dimensions;
		shader.SetVec3("chunkPos", glm::vec3(globalPos.x, 0, globalPos.y));

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
			glm::ivec3 globalPos = glm::ivec3(m_position.x * m_dimensions.x, 0, m_position.y * m_dimensions.y) + relativePos;
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

		std::cout << "Generation: " << generationEnd << ". Buffer:" << bufferEnd << '\n';
	}

private:
	unsigned int m_meshVao = 0;
	unsigned int m_verticesCount = 0;
	unsigned int m_meshVbo = 0;

	std::unordered_map<glm::ivec3, bool, VecHasher<int, 3>> m_blocks;
	glm::ivec2 m_position{};
	glm::ivec2 m_dimensions{};

	void PushFace(std::vector<float>& result, const float data[], glm::vec3 relativePos)
	{
		constexpr size_t vertLen = std::size(FRONT_FACE);

		for (size_t i = 0; i < vertLen; i += 6)
		{
			// positions
			result.push_back(data[i + 0] + relativePos.x);
			result.push_back(data[i + 1] + relativePos.y);
			result.push_back(data[i + 2] + relativePos.z);

			// normals
			result.push_back(data[i + 3]);
			result.push_back(data[i + 4]);
			result.push_back(data[i + 5]);
		}
	}
};