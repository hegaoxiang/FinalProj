#pragma once
#include<d3d11.h>
#include<DirectXMath.h>
#include <wrl/client.h>
#include"Geometry.h"
#include "DXTrace.h"
using namespace DirectX;
template<class T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

class GameObject
{
public:
	GameObject();

	// 获取位置
	DirectX::XMFLOAT3 GetPosition() const;
	// 设置缓冲区
	template<class VertexType, class IndexType>
	void SetBuffer(ID3D11Device* device, const Geometry::MeshData<VertexType, IndexType>& meshData);
	// 设置纹理
	void SetTexture(ID3D11ShaderResourceView* texture);
	// 设置矩阵
	void SetWorldMatrix(const DirectX::XMFLOAT4X4& world);
	void XM_CALLCONV SetWorldMatrix(DirectX::FXMMATRIX world);
	// 绘制
	void Draw(ID3D11DeviceContext* deviceContext);

	// 设置调试对象名
	// 若缓冲区被重新设置，调试对象名也需要被重新设置
	void SetDebugObjectName(const std::string& name);
	ComPtr<ID3D11ShaderResourceView> m_pTexture;        // 纹理
private:
	DirectX::XMFLOAT4X4 m_WorldMatrix;                  // 世界矩阵
	ComPtr<ID3D11Buffer> m_pVertexBuffer;               // 顶点缓冲区
	ComPtr<ID3D11Buffer> m_pIndexBuffer;                // 索引缓冲区
	UINT m_VertexStride;                                // 顶点字节大小
	UINT m_IndexCount;                                  // 索引数目 
};

template<class VertexType, class IndexType>
inline void GameObject::SetBuffer(ID3D11Device* device, const Geometry::MeshData<VertexType, IndexType>& meshData)
{
	/*// 释放旧资源
	m_pVertexBuffer.Reset();
	m_pIndexBuffer.Reset();
	// vertexbuffer
	D3D11_BUFFER_DESC bd;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.ByteWidth = sizeof(VertexType) * meshData.vertexVec.size();
	bd.CPUAccessFlags = 0;
	bd.Usage = D3D11_USAGE_IMMUTABLE;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = meshData.vertexVec.data();

	HR(device->CreateBuffer(&bd, &initData, m_pVertexBuffer.GetAddressOf()));

	//indexbuffer
	m_IndexCount = meshData.indexVec.size();

	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.ByteWidth = sizeof(IndexType) * meshData.indexVec.size();

	initData.pSysMem = meshData.indexVec.data();

	HR(device->CreateBuffer(&bd, &initData, m_pIndexBuffer.GetAddressOf()));*/
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

}
