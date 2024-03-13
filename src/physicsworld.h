#pragma once

#include <glm/glm.hpp>
#include <map>

#include "world.h"
#include "physicsobject.h"

class PhysicsObjectRef;

class PhysicsWorld
{
public:
	PhysicsWorld(World* world) : m_world(world) { }

	PhysicsObjectRef Reserve(const glm::vec3& position, const glm::vec3& mins, const glm::vec3& maxs);
	void Tick(float deltaTime);
	PhysicsObject& GetObject(PhysicsObjectRef ref);
	const PhysicsObject& GetObject(PhysicsObjectRef ref) const;
private:
	World* m_world;
	glm::vec3 m_gravity = glm::vec3(0, -50.f, 0);
	std::unordered_map<int, PhysicsObject> m_objects;
	int m_currentIndex = 0;
};