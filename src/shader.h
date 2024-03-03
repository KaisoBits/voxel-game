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

	static Shader Create(const std::string& vertexShaderPath, const std::string& fragmentShaderPath)
	{
		const std::string vertexShaderSource = ReadFileAsString(vertexShaderPath);
		const std::string fragmentShaderSource = ReadFileAsString(fragmentShaderPath);

		unsigned int vs = glCreateShader(GL_VERTEX_SHADER);
		unsigned int fs = glCreateShader(GL_FRAGMENT_SHADER);
		const char* srcVs = vertexShaderSource.c_str();
		glShaderSource(vs, 1, &srcVs, nullptr);
		const char* srcFs = fragmentShaderSource.c_str();
		glShaderSource(fs, 1, &srcFs, nullptr);

		char errLog[1024];
		glCompileShader(vs);
		int status = 0;
		glGetShaderiv(vs, GL_COMPILE_STATUS, &status);
		if (!status)
		{
			glGetShaderInfoLog(vs, sizeof(errLog), nullptr, errLog);
			std::cout << "VERT SHADER COMILATION ERR: " << errLog << '\n';
			return Empty();
		}

		glCompileShader(fs);

		status = 0;
		glGetShaderiv(vs, GL_COMPILE_STATUS, &status);
		if (!status)
		{
			glGetShaderInfoLog(vs, sizeof(errLog), nullptr, errLog);
			std::cout << "FRAG SHADER COMILATION ERR: " << errLog << '\n';
			return Empty();
		}

		int program = glCreateProgram();
		glAttachShader(program, vs);
		glAttachShader(program, fs);
		glLinkProgram(program);

		status = 0;
		glGetProgramiv(program, GL_LINK_STATUS, &status);
		if (!status)
		{
			glGetProgramInfoLog(program, sizeof(errLog), nullptr, errLog);
			std::cout << "SHADER PROGRAM LINK ERR: " << errLog << '\n';
			return Empty();
		}

		glDeleteShader(vs);
		glDeleteShader(fs);

		return Shader(program);
	}

	void SetMat4(const std::string& paramName, const glm::mat4& value)
	{
		int loc = glGetUniformLocation(m_programId, paramName.c_str());
		glUniformMatrix4fv(loc, 1, false, glm::value_ptr(value));
	}

	void Use() const
	{
		glUseProgram(m_programId);
	}

private:
	unsigned int m_programId = 0;

	explicit Shader(unsigned int programId) : m_programId(programId) { }

	static Shader Empty() { return Shader(0); }

	static std::string ReadFileAsString(const std::string& path) {
		const std::ifstream stream(path);
		if (!stream.is_open()) {
			std::cout << "Could not open file " << path << '\n';
			return "";
		}
		std::stringstream buffer;
		buffer << stream.rdbuf();
		return buffer.str();
	}
};