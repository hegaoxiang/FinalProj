#pragma once

#include<DirectXMath.h>

#include <DirectXCollision.h>
#include "ModelComponent.h"
#include "BasicTextureComponent.h"
#include <map>
#include <string>
using namespace DirectX;

class IEffect;

enum MyEnum
{
	IDModelComponent,
	IDBasicTextureComponent
};


class Component;
using ComponentPtr = Component*;
using ComponentID = unsigned short;
typedef  std::map<ComponentID, ComponentPtr > ComponentMap;

class GameObject
{
public:
	GameObject();
	
	void Init();
	//
	// ��ȡ��Χ��
	//

	DirectX::BoundingBox GetLocalBoundingBox() ;
	DirectX::BoundingBox GetBoundingBox() const;
	DirectX::BoundingOrientedBox GetBoundingOrientedBox() const;

	///
	/// modelComponent
	///

	// ��ȡλ��
	DirectX::XMFLOAT3 GetPosition() ;

	// ����λ�þ���
	void XM_CALLCONV SetWorldMatrix(DirectX::FXMMATRIX world);
	void SetWorldMatrix(const DirectX::XMFLOAT4X4& world);

	// ���û�����
	template<class VertexType, class IndexType>
	void SetBuffer(ID3D11Device* device, const Geometry::MeshData<VertexType, IndexType>& meshData);
	

	///
	/// basicTextureComponent
	///

	// ��������
	void SetTexture(ID3D11ShaderResourceView* texture);
	void SetTexture(ID3D11Device* device, const wchar_t* szfileName);

	// ���ò���
	void SetMaterial(Material material);


	// ����
	void Draw(ID3D11DeviceContext* deviceContext, IEffect* effect);

	// ���õ��Զ�����
	// �����������������ã����Զ�����Ҳ��Ҫ����������
	void SetDebugObjectName(const std::string& name);

	
	bool isHit = false;
private:
	// �������
	ComponentMap m_Components;
 

	DirectX::BoundingBox m_BoundingBox;
};

template<class VertexType, class IndexType>
inline void GameObject::SetBuffer(ID3D11Device* device, const Geometry::MeshData<VertexType, IndexType>& meshData)
{
	auto modelComp = dynamic_cast<ModelComponent*>(m_Components[IDModelComponent]);
	modelComp->SetBuffer(device, meshData);
}
