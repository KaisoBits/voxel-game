#pragma once

#include <glm/glm.hpp>

class PhysicsObject
{
public:
	explicit PhysicsObject(glm::vec3 position, glm::vec3 mins, glm::vec3 maxs) :
		m_position(position), m_mins(mins), m_maxs(maxs) {}

	float GetDrag() const { return drag; }
	glm::vec3 GetPosition() const { return m_position; }
	glm::vec3 GetVelocity() const { return m_velocity; }
	glm::vec3 GetMins() const { return m_mins; }
	glm::vec3 GetMaxs() const { return m_maxs; }

	void SetDrag(float value) { drag = value; }
	void SetPosition(const glm::vec3& value) { m_position = value; }
	void SetVelocity(const glm::vec3& value) { m_velocity = value; }
	void SetMins(const glm::vec3& value) { m_mins = value; }
	void SetMaxs(const glm::vec3& value) { m_maxs = value; }

private:
	float drag = 1;
	glm::vec3 m_velocity{ 0 };
	glm::vec3 m_position;
	glm::vec3 m_mins;
	glm::vec3 m_maxs;
};