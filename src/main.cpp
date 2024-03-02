#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "world.h"

constexpr int windowWidth = 800;
constexpr int windowHeight = static_cast<int>(windowWidth * (9.0/16.0));

World world(100, 100);

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

	glClearColor(0.3f, 0.3f, 0.8f, 1.0f);

	world.UpdateBlock(glm::ivec3(0), true);

	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);
		glfwPollEvents();


		glfwSwapBuffers(window);
	}

	glfwTerminate();

	return 0;
}