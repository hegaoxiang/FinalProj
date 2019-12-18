#pragma once
#include <rapidjson/prettywriter.h>
#include "DXTrace.h"
#include <d3d11.h>
#include "d3dUtil.h"
#include <wrl/client.h>
#include <memory>
using namespace rapidjson;

template<class T>
using ComPtr = Microsoft::WRL::ComPtr<T>;
using ComponentID = unsigned short;
class IEffect;
class GameObject;
class Component
{
	using Root = GameObject;
protected:
	Root* m_root;
	ComponentID m_id;
public:
	Component();
	
	
	void SetRoot(Root* gameObject) { m_root = gameObject; }

	virtual void Serialize(PrettyWriter<StringBuffer>& write) = 0;
	virtual void AntiSerialize(const char* pData) = 0;

	virtual void SetId() = 0;
	virtual ComponentID GetId() = 0;

	virtual void ApplyToDraw(ID3D11DeviceContext* deviceContext, IEffect* effect)const = 0;
};


