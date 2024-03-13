#include "physicsworld.h"
#include "physicsobjectref.h"

PhysicsObjectRef PhysicsWorld::Reserve(const glm::vec3& position, const glm::vec3& mins, const glm::vec3& maxs)
{
	int index = m_currentIndex++;

	m_objects.emplace(index, PhysicsObject(position, mins, maxs));
	return PhysicsObjectRef(index, this);
}

const PhysicsObject& PhysicsWorld::GetObject(PhysicsObjectRef ref) const
{
	return m_objects.at(ref.GetId());
}

PhysicsObject& PhysicsWorld::GetObject(PhysicsObjectRef ref) 
{
	return m_objects.at(ref.GetId());
}

void PhysicsWorld::Tick(float deltaTime)
{
	for (auto& [id, object] : m_objects)
	{
		glm::vec3 pos = object.GetPosition();
		glm::vec3 vel = object.GetVelocity();
		object.SetPosition(pos + vel * deltaTime);
		object.SetVelocity(vel + m_gravity * deltaTime);

		glm::ivec3 block = glm::ivec3(static_cast<int>(round(pos.x)), static_cast<int>(round(pos.y - 1.7f)), static_cast<int>(round(pos.z)));

		if (m_world->GetVoxel(block) != glm::ivec2(-1))
		{
			glm::vec3 v = object.GetVelocity();
			v.y = fmaxf(0, v.y);
			object.SetVelocity(v);
		}
	}
}