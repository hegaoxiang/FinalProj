#pragma once
#include <functional>
#include <map>
#include <memory>
#include <string>
#include "GameObejct.h"

using ComponentCreator = Component*(*)();
using Tag = UINT;
using ActorComponentCreatorMap = std::map<Tag, ComponentCreator>;

using StrongActorPtr = std::shared_ptr<GameObject>;


class  ActorFactory
{
	
	ActorId	m_lastActorId;
protected:
	ActorComponentCreatorMap m_ComponentCreator;
public:
	ActorFactory();
	ActorFactory& GetActorFactory() ;
	StrongActorPtr CreateActor(const char* actorInfo);
protected:
	Component* CreateComponent(const char* pData);

private:
	ActorId GetNextActorId() { ++m_lastActorId; return m_lastActorId; }
};
static ActorFactory g_ActorFactoryIns;

