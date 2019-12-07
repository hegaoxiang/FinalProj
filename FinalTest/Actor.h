#pragma once
#include "Vertex.h"
#include "ActorComponent.h"
typedef unsigned long ActorId;
class Actor
{
	ActorId	m_id;
protected:
	Vec3 m_positon;
public:
	const Vec3& GetPosition()const { return m_positon; }

	const ActorId GetId()const { return m_id; }
};

