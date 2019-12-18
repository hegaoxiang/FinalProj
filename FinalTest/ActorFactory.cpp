#include "ActorFactory.h"
#include <rapidjson\document.h>
#include <rapidjson\pointer.h>

using namespace rapidjson;

Component* CreateModelComp()
{
	return new ModelComponent();
}
Component* CreateBasicTexComp()
{
	return new BasicTextureComponent();
}
ActorFactory::ActorFactory()
{
	m_lastActorId = 0;
	m_ComponentCreator.insert(std::pair<Tag, ComponentCreator>(IDModelComponent, CreateModelComp));
	m_ComponentCreator.insert(std::pair<Tag, ComponentCreator>(IDBasicTextureComponent, CreateBasicTexComp));
}
ActorFactory& ActorFactory::GetActorFactory()
{
	return g_ActorFactoryIns; 
}
StrongActorPtr ActorFactory::CreateActor(const char* actorInfo)
{
	StrongGameObjectPtr objectPtr = std::make_shared<GameObject>();

	Document d;
	d.Parse(actorInfo);

	auto name = Pointer("/name").Get(d);
	assert(name);

	objectPtr->SetName(name->GetString());
	objectPtr->SetId(GetNextActorId());
	///
	/// Components
	///
	auto componentSJson = Pointer("/ComponentsAttribute").Get(d);

	for (auto& compItem : componentSJson->GetArray())
	{
		StringBuffer sb;
		Writer<StringBuffer> writer(sb);
		compItem.Accept(writer);
		objectPtr->AddCompent(CreateComponent(sb.GetString()));
	}

	return objectPtr;
}

Component* ActorFactory::CreateComponent(const char* pData)
{
	Document d;
	d.Parse(pData);

	auto tag = Pointer("/ComponentTag").Get(d);
	
	auto f = m_ComponentCreator[tag->GetUint()];
	Component* pComp(f());

	auto temp = f();
	temp->AntiSerialize(pData);
	return temp;
	
}
