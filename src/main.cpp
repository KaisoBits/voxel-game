#include <iostream>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

#include "world.h"
#include "shader.h"
#include "camera.h"

constexpr int windowWidth = 800;
constexpr int windowHeight = static_cast<int>(windowWidth * (9.0 / 16.0));

Camera mainCam(75, static_cast<float>(windowWidth) / windowHeight);
constexpr float mouseSensitivity = 0.4f;

void windowSizeChangeCallback(GLFWwindow* window, int newWidth, int newHeight);
void handleCameraMovement(GLFWwindow* window, float deltaTime);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);

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

	mainCam.SetPosition(glm::vec3(0, 0, 10));

	glViewport(0, 0, windowWidth, windowHeight);
	glfwSetWindowSizeCallback(window, windowSizeChangeCallback);
	glfwSetCursorPosCallback(window, mouseCallback);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glClearColor(52 / 255.0f, 152 / 255.0f, 219 / 255.0f, 1.0f);

	Shader shader = Shader::Create("shaders/main.vert", "shaders/main.frag");
	shader.Use();

	World world(100, 100);

	glm::mat4 perspective = glm::perspective(90.0f, 16.0f / 9.0f, 0.1f, 3000.0f);

	double period = 0.00;
	double lastSpawn = glfwGetTime();

	size_t x = 1;
	size_t y = 1;
	size_t z = 1;
	bool done = false;

	double lastTime = glfwGetTime();
	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glfwPollEvents();

		double now = glfwGetTime();
		double deltaTime = fmin(now - lastTime, 0.3f);
		lastTime = now;

		if (now - lastSpawn > period && !done)
		{
			lastSpawn = now;

			if (x > 90)
			{
				x = 1;
				y++;
			}
			if (y > 20)
			{
				y = 1;
				z++;
			}
			if (z > 90)
			{
				done = true;
			}

			x++;

			world.UpdateVoxel(glm::ivec3(x, y, z), true);
		}

		handleCameraMovement(window, static_cast<float>(deltaTime));

		glm::mat4 model = glm::mat4(1); // glm::rotate(glm::mat4(1.0f), static_cast<float>(glfwGetTime()) / 2.0f, glm::vec3(0, 1.0f, 0));
		glm::mat4 view = mainCam.GetMatrix(); // glm::translate(glm::mat4(1), -glm::vec3(0, sin(0) * 3, 3));

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

void handleCameraMovement(GLFWwindow* window, float deltaTime)
{
	constexpr glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

	float speed = (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) ?
		100.0f : 40.0f;

	glm::vec3 position = mainCam.GetPosition();

	const glm::vec3 forwardVectorJustYaw = mainCam.ForwardJustYaw();

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		position += forwardVectorJustYaw * deltaTime * speed;
	else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		position -= forwardVectorJustYaw * deltaTime * speed;

	const glm::vec3 right = glm::cross(forwardVectorJustYaw, up);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		position += right * deltaTime * speed;
	else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		position -= right * deltaTime * speed;

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		position += up * deltaTime * speed;
	else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		position -= up * deltaTime * speed;

	mainCam.SetPosition(position);
}

void windowSizeChangeCallback(GLFWwindow* window, int newWidth, int newHeight)
{
	if (newWidth == 0 || newHeight == 0) // ignore minimizing
		return;

	glViewport(0, 0, newWidth, newHeight);
	mainCam.SetAspectRatio(static_cast<float>(newWidth) / static_cast<float>(newHeight));
}

double lastX = -1;
double lastY = -1;
void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	glm::vec2 camRotation = mainCam.GetRotation();

	camRotation.x += static_cast<float>(xpos - lastX) * mouseSensitivity;
	camRotation.y = fmin(fmax(camRotation.y + static_cast<float>(ypos - lastY) * mouseSensitivity, -89.0f), 89.0f);

	mainCam.SetRotation(camRotation);

	lastX = xpos;
	lastY = ypos;
}