#pragma once

#include<DirectXMath.h>
#include <DirectXCollision.h>
#include "ModelComponent.h"
#include "BasicTextureComponent.h"
#include <map>
#include <memory>
#include <string>
#include <rapidjson/prettywriter.h>
using namespace rapidjson;
using namespace DirectX;

class IEffect;

enum MyEnum
{
	IDModelComponent,
	IDBasicTextureComponent
};


class Component;
using StrongCompnentPtr = std::shared_ptr<Component>;
using ComponentID = unsigned short;
using ActorId = unsigned short;
using StrongGameObjectPtr = std::shared_ptr<GameObject>;
typedef  std::map<ComponentID, StrongCompnentPtr > ComponentMap;


class GameObject
{
public:
	GameObject();

	void SetId(int id) { m_id = id; }
	void Init();
	//
	// 获取包围盒
	//

	DirectX::BoundingBox GetLocalBoundingBox() ;
	DirectX::BoundingBox GetBoundingBox() const;
	DirectX::BoundingOrientedBox GetBoundingOrientedBox() const;

	///
	/// modelComponent
	///

	// 获取位置
	DirectX::XMFLOAT3 GetPosition() ;

	// 设置位置矩阵
	void XM_CALLCONV SetWorldMatrix(DirectX::FXMMATRIX world);
	void SetWorldMatrix(const DirectX::XMFLOAT4X4& world);

	// 设置缓冲区
	template<class VertexType, class IndexType>
	void SetBuffer(ID3D11Device* device, const Geometry::MeshData<VertexType, IndexType>& meshData);
	

	///
	/// basicTextureComponent
	///

	// 设置纹理
	void SetTexture(ID3D11ShaderResourceView* texture);
	void SetTexture(ID3D11Device* device, const wchar_t* szfileName);

	// 设置材质
	void SetMaterial(Material material);

	void SetName(const std::string& str) { m_name = str; }
	// 绘制
	void Draw(ID3D11DeviceContext* deviceContext, IEffect* effect);

	// 设置调试对象名
	// 若缓冲区被重新设置，调试对象名也需要被重新设置
	void SetDebugObjectName(const std::string& name);

	
	bool isHit = false;

	///
	/// serialize
	///
	void Serialize(PrettyWriter<StringBuffer>& write);
	void SetBufferAttr(int vertexType, int primitive, std::initializer_list<float>details);

	void AddCompent(Component* pComp);
	///
private:
	ActorId m_id;
	std::string m_name;
	// 组件容器
	ComponentMap m_Components;

	DirectX::BoundingBox m_BoundingBox;
};

template<class VertexType, class IndexType>
inline void GameObject::SetBuffer(ID3D11Device* device, const Geometry::MeshData<VertexType, IndexType>& meshData)
{
	auto modelComp = dynamic_cast<ModelComponent*>(m_Components[IDModelComponent].get());
	modelComp->SetBuffer(device, meshData);
}
