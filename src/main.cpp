#include <iostream>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

#include <PerlinNoise.hpp>

#include "world.h"
#include "shader.h"
#include "camera.h"
#include "texture.h"

constexpr int windowWidth = 800;
constexpr int windowHeight = static_cast<int>(windowWidth * (9.0 / 16.0));

Camera mainCam(75.0f, static_cast<float>(windowWidth) / windowHeight);
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

	Texture blocks = Texture::Load("resources/textures/minecraft-textures.png", false);
	blocks.Use(0);

	Shader shader = Shader::Create("shaders/main.vert", "shaders/main.frag");
	shader.Use();

	World world(glm::ivec3(16, 16, 16));

	double period = 0.00;
	double lastSpawn = glfwGetTime();

	const siv::PerlinNoise::seed_type seed = 123456u;
	const siv::PerlinNoise perlin{ seed };

	for (size_t x = 0; x <= 100; x++)
	{
		for (size_t z = 0; z <= 100; z++)
		{
			const double bonusY = perlin.noise2D(x / 15.0, z / 15.0);
			for (size_t y = 0; y <= 30 + static_cast<size_t>(bonusY * 20); y++)
			{
				if (y > 30)
					world.UpdateVoxel(glm::ivec3(x, y, z), glm::ivec2(13, 9));
				else
					world.UpdateVoxel(glm::ivec3(x, y, z), glm::ivec2(7, 14));
			}
		}
	}

	double lastTime = glfwGetTime();
	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glfwPollEvents();

		double now = glfwGetTime();
		double deltaTime = fmin(now - lastTime, 0.3);
		lastTime = now;

		handleCameraMovement(window, static_cast<float>(deltaTime));

		int windowWidth, windowHeight;
		glfwGetWindowSize(window, &windowWidth, &windowHeight);

		glm::mat4 model = glm::mat4(1); // glm::rotate(glm::mat4(1.0f), static_cast<float>(glfwGetTime()) / 2.0f, glm::vec3(0, 1.0f, 0));
		glm::mat4 view = mainCam.GetMatrix(); // glm::translate(glm::mat4(1), -glm::vec3(0, sin(0) * 3, 3));
		glm::mat4 perspective = glm::perspective(glm::radians(mainCam.GetFovY()), static_cast<float>(windowWidth) / windowHeight, 0.1f, 1000.0f);

		shader.SetMat4("model", model);
		shader.SetMat4("view", view);
		shader.SetMat4("perspective", perspective);

		world.ApplyChanges();
		world.Draw(shader);

		glm::vec3 camPos = mainCam.GetPosition() + glm::vec3(0.5f);

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_RELEASE)
		{
			for (int x = -2; x < 2; x++)
				for (int y = -2; y <= 2; y++)
					for (int z = -2; z <= 2; z++)
						world.UpdateVoxel(glm::ivec3(camPos) + glm::ivec3(x, y, z), glm::ivec2(-1));
		}

		glfwSwapBuffers(window);
	}

	glfwTerminate();

	return 0;
}

void handleCameraMovement(GLFWwindow* window, float deltaTime)
{
	constexpr glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

	float speed = (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) ?
		4.0f : 40.0f;

	glm::vec3 position = mainCam.GetPosition();

	const glm::vec3 forwardVectorJustYaw = mainCam.ForwardJustYaw();

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		position += forwardVectorJustYaw * deltaTime * speed;
	else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		position -= forwardVectorJustYaw * deltaTime * speed;

	const glm::vec3 right = mainCam.Right();
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