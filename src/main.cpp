#include <iostream>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

#include <PerlinNoise.hpp>

#include "world.h"
#include "physicsworld.h"
#include "physicsobjectref.h"
#include "shader.h"
#include "camera.h"
#include "texture.h"

int windowWidth = 800;
int windowHeight = static_cast<int>(windowWidth * (9.0 / 16.0));

Camera mainCam(75.0f, static_cast<float>(windowWidth) / windowHeight);
constexpr float mouseSensitivity = 0.4f;

void windowSizeChangeCallback(GLFWwindow* window, int newWidth, int newHeight);
void handleCameraMovement(GLFWwindow* window, PhysicsObject& physics);
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

	mainCam.SetPosition(glm::vec3(70, 60, 70));

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
	PhysicsWorld physicsWorld(&world);
	PhysicsObjectRef mainCamPhysics = physicsWorld.Reserve(mainCam.GetPosition(), glm::vec3(-1), glm::vec3(1));

	const siv::PerlinNoise::seed_type seed = 123456u;
	const siv::PerlinNoise perlin{ seed };

	for (size_t x = 0; x <= 100; x++)
	{
		for (size_t z = 0; z <= 100; z++)
		{
			const double bonusY = perlin.noise2D(x / 35.0, z / 35.0);
			for (size_t y = 20; y <= 30 + static_cast<size_t>(bonusY * 20); y++)
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

		PhysicsObject& cameraPhysics = mainCamPhysics.Object();
		handleCameraMovement(window, cameraPhysics);

		world.ApplyChanges();
		physicsWorld.Tick(deltaTime);

		mainCam.SetPosition(cameraPhysics.GetPosition());

		glm::mat4 model = glm::mat4(1);
		glm::mat4 view = mainCam.GetMatrix();
		glm::mat4 perspective = glm::perspective(glm::radians(mainCam.GetFovY()), static_cast<float>(windowWidth) / windowHeight, 0.1f, 1000.0f);

		shader.SetMat4("model", model);
		shader.SetMat4("view", view);
		shader.SetMat4("perspective", perspective);

		world.Draw(shader);

		glm::vec3 camPos = mainCam.GetPosition() + glm::vec3(0.5f);
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
		{
			for (int x = -2; x < 2; x++)
				for (int y = -2; y <= 2; y++)
					for (int z = -2; z <= 2; z++)
						world.UpdateVoxel(glm::ivec3(camPos) + glm::ivec3(x, y, z), glm::ivec2(-1));
		}

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS)
		{
			for (int x = -2; x < 2; x++)
				for (int y = -2; y <= 2; y++)
					for (int z = -2; z <= 2; z++)
						world.UpdateVoxel(glm::ivec3(camPos) + glm::ivec3(x, y, z), glm::ivec2(0, 1));
		}

		glfwSwapBuffers(window);
	}

	glfwTerminate();

	return 0;
}

float lastJump = -1;
void handleCameraMovement(GLFWwindow* window, PhysicsObject& physics)
{
	constexpr glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

	float speed = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) ?
		2.0f : 5.0f;

	glm::vec3 velocity(0);

	const glm::vec3 forwardVectorJustYaw = mainCam.ForwardJustYaw();

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		velocity += forwardVectorJustYaw;
	else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		velocity -= forwardVectorJustYaw;

	const glm::vec3 right = mainCam.Right();
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		velocity += right;
	else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		velocity -= right;

	glm::vec3 walkDir = velocity != glm::vec3(0) ? glm::normalize(velocity) * speed : glm::vec3(0);

	float y = physics.GetVelocity().y;
	float now = glfwGetTime();
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && now - lastJump > 0.4f)
	{
		y = 13.0f;
		lastJump = now;
	}

	physics.SetVelocity(glm::vec3(walkDir.x, y, walkDir.z));
}

void windowSizeChangeCallback(GLFWwindow* window, int newWidth, int newHeight)
{
	if (newWidth == 0 || newHeight == 0) // ignore minimizing
		return;

	windowWidth = newWidth;
	windowHeight = newHeight;

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