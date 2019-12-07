#include "ActorFactory.h"

StrongActorPtr ActorFactory::CreateActor(const char* actorResource)
{
	return StrongActorPtr();
}

StrongActorComponentPtr ActorFactory::CreateComponent(const char* pData)
{
	return StrongActorComponentPtr();
}
