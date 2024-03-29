#pragma once
#include "Component.h"
#include"Geometry.h"


class ModelComponent :
	public Component
{
public:
	ModelComponent() : m_isOutside(false), m_IndexCount(), m_VertexStride(),m_Position() {
		XMStoreFloat4x4(&m_WorldMatrix, DirectX::XMMatrixIdentity()); 
		SetId();
	}

	DirectX::XMFLOAT4X4 GetWorldMatrix()const;
	// 获取位置
	DirectX::XMFLOAT3 GetPosition() const;

	// 设置位置矩阵
	void XM_CALLCONV SetWorldMatrix(DirectX::FXMMATRIX world);
	void SetWorldMatrix(const DirectX::XMFLOAT4X4& world);
	void SetPosition(const DirectX::XMFLOAT3& position);

	// 设置缓冲区
	template<class VertexType, class IndexType>
	void SetBuffer(ID3D11Device* device, const Geometry::MeshData<VertexType, IndexType>& meshData);

	void ApplyToDraw(ID3D11DeviceContext* deviceContext, IEffect* effect) const override;

	// 得到索引数目
	UINT GetIndexCount()const;

	ComponentID GetId() override;
	
	// serialize
	void Serialize(PrettyWriter<StringBuffer>& write) override;
	
	void SetBufferAttr(int vertexType,int primitive,std::initializer_list<float>details);


	void AntiSerialize(const char* pData) override;

private:
	void SetId() override;

	DirectX::XMFLOAT4X4 m_WorldMatrix;                  // 世界矩阵

	ComPtr<ID3D11Buffer> m_pVertexBuffer;               // 顶点缓冲区
	ComPtr<ID3D11Buffer> m_pIndexBuffer;                // 索引缓冲区
	UINT m_VertexStride;                                // 顶点字节大小
	UINT m_IndexCount;                                  // 索引数目
	
	// serialize
	DirectX::XMFLOAT3 m_Position;
	bool m_isOutside;

	int m_Vertextype;
	int m_Primitive;
	
	std::vector<float> m_MeshAttrs;
};

template<class VertexType, class IndexType>
inline void ModelComponent::SetBuffer(ID3D11Device* device, const Geometry::MeshData<VertexType, IndexType>& meshData)
{
	m_isOutside = false;

	// 释放旧资源
	m_pVertexBuffer.Reset();
	m_pIndexBuffer.Reset();

	// 设置顶点缓冲区描述
	m_VertexStride = sizeof(VertexType);
	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = (UINT)meshData.vertexVec.size() * m_VertexStride;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	// 新建顶点缓冲区
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = meshData.vertexVec.data();
	HR(device->CreateBuffer(&vbd, &InitData, m_pVertexBuffer.GetAddressOf()));


	// 设置索引缓冲区描述
	m_IndexCount = (UINT)meshData.indexVec.size();
	D3D11_BUFFER_DESC ibd;
	ZeroMemory(&ibd, sizeof(ibd));
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = m_IndexCount * sizeof(IndexType);
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	// 新建索引缓冲区
	InitData.pSysMem = meshData.indexVec.data();
	HR(device->CreateBuffer(&ibd, &InitData, m_pIndexBuffer.GetAddressOf()));

	D3D11SetDebugObjectName(m_pVertexBuffer.Get(), "ModelConpVertexBuffer");
	D3D11SetDebugObjectName(m_pIndexBuffer.Get(), "ModelConpIndexBuffer");

}
