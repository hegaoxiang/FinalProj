#pragma once

#include<DirectXMath.h>

#include <DirectXCollision.h>
#include "ModelComponent.h"
#include "BasicTextureComponent.h"
#include <map>
#include <string>
using namespace DirectX;

class IEffect;
class Component;
using ComponentPtr = Component*;
using ComponentMap = std::map<const std::string&, ComponentPtr >;

class GameObject
{
public:
	GameObject();
	
	void Init();
	//
	// 获取包围盒
	//

	DirectX::BoundingBox GetLocalBoundingBox() const;
	DirectX::BoundingBox GetBoundingBox() const;
	DirectX::BoundingOrientedBox GetBoundingOrientedBox() const;


	
	// 绘制
	void Draw(ID3D11DeviceContext* deviceContext, IEffect* effect);

	// 设置调试对象名
	// 若缓冲区被重新设置，调试对象名也需要被重新设置
	void SetDebugObjectName(const std::string& name);

	
private:
	// 组件容器
	ComponentMap m_Components;
 
	

	DirectX::BoundingBox m_BoundingBox;
};

