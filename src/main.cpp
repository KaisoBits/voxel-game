#include <iostream>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

#include "world.h"
#include "shader.h"

constexpr int windowWidth = 800;
constexpr int windowHeight = static_cast<int>(windowWidth * (9.0 / 16.0));

int main(int argc, char** argv)
{
	if (!glfwInit())
	{
		std::cout << "Failed to initialize GLFW\n";
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Voxel", nullptr, nullptr);
	if (!window)
	{
		std::cout << "Failed to initialize a GLFW window\n";
		return -2;
	}

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
	{
		std::cout << "Failed to initialize OpenGL\n";
		return -3;
	}

	glViewport(0, 0, windowWidth, windowHeight);
	glClearColor(0.3f, 0.3f, 0.8f, 1.0f);

	Shader shader = Shader::Create("shaders/main.vert", "shaders/main.frag");
	shader.Use();

	World world(100, 100);
	world.UpdateVoxel(glm::ivec3(0), true);

	glm::mat4 perspective = glm::perspective(90.0f, 16.0f / 9.0f, 0.1f, 1000.0f);

	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);
		glfwPollEvents();

		glm::mat4 model = glm::rotate(glm::mat4(1.0f), static_cast<float>(glfwGetTime()) / 2.0f, glm::vec3(0, 1.0f, 0));
		glm::mat4 view = glm::translate(glm::mat4(1), -glm::vec3(0, sin(glfwGetTime()) * 3, 3));

		shader.SetMat4("model", model);
		shader.SetMat4("view", view);
		shader.SetMat4("perspective", perspective);

		world.ApplyChanges();
		world.Draw();

		glfwSwapBuffers(window);
	}

	glfwTerminate();

	return 0;
}