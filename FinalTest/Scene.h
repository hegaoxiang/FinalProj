#pragma once
#include "GameObejct.h"
#include <vector>
#include <rapidjson/prettywriter.h>
#include <rapidjson/document.h>
#include <memory>
#include <rapidjson/filereadstream.h>
using namespace rapidjson;


class Scene
{
public:
	void Serialize(PrettyWriter<StringBuffer>& write);

	void AntiSerialize(const std::string& scenePath);

	void AddStaticObject(StrongGameObjectPtr gameObject) { m_staticObjects.push_back(StrongGameObjectPtr(gameObject)); }

	std::vector<StrongGameObjectPtr> GetObjects() { return m_staticObjects; }

private:
	std::vector<StrongGameObjectPtr> m_staticObjects;
};

