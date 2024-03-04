#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
public:
	Camera(float fovY, float aspectRatio) : m_fovY(fovY), m_aspectRatio(aspectRatio) {}

	glm::mat4 GetMatrix() const
	{
		const glm::mat4 viewMatrix =
			glm::translate(
				glm::rotate(
					glm::rotate(
						glm::mat4(1.0f),
						glm::radians(m_rotation.y), glm::vec3(1.0f, 0.0f, 0.0f)
					),
					glm::radians(m_rotation.x), glm::vec3(0.0f, 1.0f, 0.0f)
				), -m_position);

		return viewMatrix;
	}

	[[nodiscard]] glm::vec2 GetRotation() const { return m_rotation; }
	void SetRotation(glm::vec2 rotation) { m_rotation = rotation; }

	[[nodiscard]] glm::vec3 GetPosition() const { return m_position; }
	void SetPosition(glm::vec3 position) { m_position = position; }

	[[nodiscard]] float GetFovY() const { return m_fovY; }
	void SetFovY(float fovY) { m_fovY = fovY; }

	[[nodiscard]] float GetAspectRatio() const { return m_aspectRatio; }
	void SetAspectRatio(float aspectRatio) { m_aspectRatio = aspectRatio; }

	[[nodiscard]] glm::vec3 Forward()
	{
		glm::vec3 forwardVector(
			cos(glm::radians(m_rotation.x + 90)) * cos(glm::radians(m_rotation.y)),
			sin(glm::radians(m_rotation.y)),
			sin(glm::radians(m_rotation.x + 90)) * cos(glm::radians(m_rotation.y)));
		return glm::normalize(-forwardVector);
	}

	[[nodiscard]] glm::vec3 ForwardJustYaw()
	{
		glm::vec3 forwardVectorJustYaw(
			cos(glm::radians(m_rotation.x + 90)),
			0,
			sin(glm::radians(m_rotation.x + 90)));
		return glm::normalize(-forwardVectorJustYaw);
	}

	[[nodiscard]] glm::vec3 Up()
	{
		return { 0, 1, 0 };
	}

	[[nodiscard]] glm::vec3 Right()
	{
		return glm::cross(ForwardJustYaw(), Up());
	}

private:
	glm::vec2 m_rotation = glm::vec2(0, 0);
	glm::vec3 m_position = glm::vec3(0, 0, 0);
	float m_fovY;
	float m_aspectRatio;
};
