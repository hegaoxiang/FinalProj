#pragma once
#include "DXTrace.h"
#include <d3d11.h>
#include <wrl/client.h>
template<class T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

class IEffect;
class GameObject;
class Component
{
	using Root = GameObject;
private:
	Root* m_root;

public:
	Component() : m_root(nullptr){ }
	void SetRoot(Root* gameObject) { m_root = gameObject; }

	virtual void ApplyToDraw(ID3D11DeviceContext* deviceContext, IEffect* effect)const = 0;
};

