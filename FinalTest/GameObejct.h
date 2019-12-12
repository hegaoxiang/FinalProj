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
	// ��ȡ��Χ��
	//

	DirectX::BoundingBox GetLocalBoundingBox() const;
	DirectX::BoundingBox GetBoundingBox() const;
	DirectX::BoundingOrientedBox GetBoundingOrientedBox() const;


	
	// ����
	void Draw(ID3D11DeviceContext* deviceContext, IEffect* effect);

	// ���õ��Զ�����
	// �����������������ã����Զ�����Ҳ��Ҫ����������
	void SetDebugObjectName(const std::string& name);

	
private:
	// �������
	ComponentMap m_Components;
 
	

	DirectX::BoundingBox m_BoundingBox;
};

