#pragma once
#include <functional>
#include <map>
#include <memory>
#include <string>
#include "Actor.h"
using ActorComponentCreator = ActorComponent*(*)();
using ActorComponentCreatorMap = std::map<std::string, ActorComponentCreator>;

using StrongActorPtr = std::shared_ptr<Actor>;
using StrongActorComponentPtr = std::shared_ptr<ActorComponent>;

class ActorFactory
{
	ActorId	m_lastActorId;
protected:
	ActorComponentCreatorMap m_actorComponentCreators;
public:
	ActorFactory();
	StrongActorPtr CreateActor(const char* actorResource);
protected:
	virtual StrongActorComponentPtr CreateComponent(const char* pData);

private:
	ActorId GetNextActorId() { ++m_lastActorId; return m_lastActorId; }
};

