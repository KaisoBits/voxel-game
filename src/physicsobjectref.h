#pragma once

#include "physicsworld.h"
#include "physicsobject.h"

class PhysicsObjectRef
{
public:
	explicit PhysicsObjectRef(int id, PhysicsWorld* world) : m_id(id), m_world(world){ }
	int GetId() const { return m_id; }

	PhysicsObject& Object() const { return m_world->GetObject(*this); }

private:
	int m_id;
	PhysicsWorld* m_world;
};