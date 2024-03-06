#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader
{
public:
	Shader() = delete;

	static Shader Create(const std::string& vertexShaderPath,
		const std::string& fragmentShaderPath);

	void SetMat4(const std::string& paramName, const glm::mat4& value);

	void SetVec3(const std::string& paramName, const glm::vec3& value);

	void Use() const;

private:
	unsigned int m_programId = 0;

	explicit Shader(unsigned int programId) : m_programId(programId) { }

	static Shader Empty() { return Shader(0); }

	static std::string ReadFileAsString(const std::string& path);
};