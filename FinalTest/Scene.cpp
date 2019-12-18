#include "Scene.h"
#include "rapidjson/pointer.h"
#include "ActorFactory.h"
extern ActorFactory g_ActorFactoryIns;
void Scene::Serialize(PrettyWriter<StringBuffer>& write)
{
	write.StartObject();

	write.Key("GameObjects");
	write.StartArray();
	for (auto& item : m_staticObjects)
	{
		item->Serialize(write);
	}
	write.EndArray();

	write.EndObject();
}

void Scene::AntiSerialize(const std::string& scenePath)
{
	FILE* fp;
	
	fopen_s(&fp,scenePath.c_str(), "rb"); // 非 Windows 平台使用 "r"
	assert(fp != 0);

	char *readBuffer = new char[65536];
	FileReadStream is(fp, readBuffer, sizeof(readBuffer));

	Document d;
	
	d.ParseStream(is);

	delete []readBuffer;
	fclose(fp);


	for (Value& item : d["GameObjects"].GetArray())
	{
		StringBuffer sb;
		Writer<StringBuffer> writer(sb);
	
		item.Accept(writer);
		//item.GetObjectW().Accept(writer);
		//ActorFactory::CreateActor();
		AddStaticObject(g_ActorFactoryIns.CreateActor(sb.GetString()));
	}
}
