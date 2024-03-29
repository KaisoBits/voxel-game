#include <glm/glm.hpp>
#include <glad/glad.h>
#include <unordered_map>
#include <vector>
#include <chrono>
#include <iostream>
#include <execution>
#include <mutex>

#include "vechasher.h"
#include "shader.h"
#include "blockprovider.h"
#include "blockdata.h"

#include "chunk.h"

Chunk::Chunk(glm::ivec3 dimensions, glm::ivec3 position)
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

Chunk::Chunk(Chunk&& other) noexcept
{
	m_position = other.m_position;
	m_dimensions = other.m_dimensions;

	m_meshVbo = other.m_meshVbo;
	other.m_meshVbo = 0;
	m_meshVao = other.m_meshVao;
	other.m_meshVao = 0;
	m_verticesCount = other.m_verticesCount;
	m_blocks = std::move(other.m_blocks);
}

Chunk& Chunk::operator=(Chunk&& other) noexcept
{
	if (&other == this)
		return *this;

	m_position = other.m_position;
	m_dimensions = other.m_dimensions;

	if (m_meshVbo != 0)
		glDeleteBuffers(1, &m_meshVbo);
	m_meshVbo = other.m_meshVbo;
	other.m_meshVbo = 0;

	if (m_meshVao != 0)
		glDeleteBuffers(1, &m_meshVao);
	m_meshVao = other.m_meshVao;
	other.m_meshVao = 0;

	m_verticesCount = other.m_verticesCount;
	m_blocks = std::move(other.m_blocks);

	return *this;
}

Chunk::~Chunk()
{
	if (m_meshVbo != 0)
		glDeleteBuffers(1, &m_meshVbo);

	if (m_meshVao != 0)
		glDeleteBuffers(1, &m_meshVao);
}

bool Chunk::UpdateVoxel(const glm::ivec3& coordinate, const glm::ivec2& textureCoordinate)
{
	auto it = m_blocks.find(coordinate);

	if (textureCoordinate == glm::ivec2(-1))
	{
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
		if (it != m_blocks.end() && it->second == textureCoordinate)
			return false;

		m_blocks[coordinate] = textureCoordinate;
		return true;
	}
}

glm::vec2 Chunk::GetVoxel(const glm::ivec3& coordinate) const
{
	auto it = m_blocks.find(coordinate);
	return it == m_blocks.end() ? glm::ivec2(-1) : it->second;
}

void Chunk::Draw(Shader& shader) const
{
	if (m_verticesCount == 0) // Fully filled, fully covered chunk
		return;

	glm::ivec3 globalPos = m_position * m_dimensions;
	shader.SetVec3("chunkPos", globalPos);

	glBindVertexArray(m_meshVao);
	glDrawArrays(GL_TRIANGLES, 0, m_verticesCount);
}

bool Chunk::GenerateMesh(const IBlockProvider& blockProvider)
{
	using namespace std::chrono;
	using namespace std::chrono_literals;

	if (m_blocks.size() == 0)
	{
		return false; // will get deleted by the world
	}

	unsigned int vertCount = 0;

	glBindVertexArray(m_meshVao);
	glBindBuffer(GL_ARRAY_BUFFER, m_meshVbo);

	std::vector<float> vertexData;

	auto generationStart = steady_clock::now();

	std::mutex mutex;

	std::for_each(std::execution::par, m_blocks.begin(), m_blocks.end(),
		[&](const auto& voxelEntry) {
			auto& [relativePos, textureCoord] = voxelEntry;

			std::vector<float> localVertexData;
			glm::ivec3 globalPos = m_position * m_dimensions + relativePos;
			if (blockProvider.GetVoxel(globalPos + glm::ivec3(0, 0, 1)) == glm::ivec2(-1))
				PushFace(localVertexData, FRONT_FACE, relativePos, textureCoord);
			if (blockProvider.GetVoxel(globalPos + glm::ivec3(0, 0, -1)) == glm::ivec2(-1))
				PushFace(localVertexData, BACK_FACE, relativePos, textureCoord);
			if (blockProvider.GetVoxel(globalPos + glm::ivec3(0, 1, 0)) == glm::ivec2(-1))
				PushFace(localVertexData, TOP_FACE, relativePos, textureCoord);
			if (blockProvider.GetVoxel(globalPos + glm::ivec3(0, -1, 0)) == glm::ivec2(-1))
				PushFace(localVertexData, BOTTOM_FACE, relativePos, textureCoord);
			if (blockProvider.GetVoxel(globalPos + glm::ivec3(1, 0, 0)) == glm::ivec2(-1))
				PushFace(localVertexData, RIGHT_FACE, relativePos, textureCoord);
			if (blockProvider.GetVoxel(globalPos + glm::ivec3(-1, 0, 0)) == glm::ivec2(-1))
				PushFace(localVertexData, LEFT_FACE, relativePos, textureCoord);

			if (!localVertexData.empty())
			{
				std::lock_guard<std::mutex> guard(mutex);
				vertexData.insert(vertexData.end(), localVertexData.begin(), localVertexData.end());
			}
		});

	auto generationEnd = duration_cast<microseconds>(steady_clock::now() - generationStart);

	auto bufferStart = steady_clock::now();
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertexData.size(), vertexData.data(), GL_DYNAMIC_DRAW);
	auto bufferEnd = duration_cast<microseconds>(steady_clock::now() - bufferStart);

	m_verticesCount = static_cast<unsigned int>(vertexData.size());

	return true;

	// std::cout << "Generation: " << generationEnd << ". Buffer:" << bufferEnd << '\n';
}

void Chunk::PushFace(std::vector<float>& result, const float data[], const glm::vec3& relativePos, const glm::ivec2& textureCoord)
{
	constexpr size_t vertLen = std::size(FRONT_FACE);

	for (size_t i = 0; i < vertLen; i += 8)
	{
		// positions
		result.push_back(data[i + 0] + relativePos.x);
		result.push_back(data[i + 1] + relativePos.y);
		result.push_back(data[i + 2] + relativePos.z);

		// normals
		result.push_back(data[i + 3]);
		result.push_back(data[i + 4]);
		result.push_back(data[i + 5]);

		// uv
		constexpr float tileX = 64.0f;
		constexpr float tileY = 32.0f;

		float x = 1 / tileX;
		float y = 1 / tileY;
		result.push_back(data[i + 6] / tileX + x * textureCoord.x);
		result.push_back(data[i + 7] / tileY + y * textureCoord.y);
	}
}