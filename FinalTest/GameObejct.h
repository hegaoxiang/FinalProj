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

	// ��ȡλ��
	DirectX::XMFLOAT3 GetPosition() const;
	// ���û�����
	template<class VertexType, class IndexType>
	void SetBuffer(ID3D11Device* device, const Geometry::MeshData<VertexType, IndexType>& meshData);
	// ��������
	void SetTexture(ID3D11ShaderResourceView* texture);
	// ���þ���
	void SetWorldMatrix(const DirectX::XMFLOAT4X4& world);
	void XM_CALLCONV SetWorldMatrix(DirectX::FXMMATRIX world);
	// ����
	void Draw(ID3D11DeviceContext* deviceContext);

	// ���õ��Զ�����
	// �����������������ã����Զ�����Ҳ��Ҫ����������
	void SetDebugObjectName(const std::string& name);
	ComPtr<ID3D11ShaderResourceView> m_pTexture;        // ����
private:
	DirectX::XMFLOAT4X4 m_WorldMatrix;                  // �������
	ComPtr<ID3D11Buffer> m_pVertexBuffer;               // ���㻺����
	ComPtr<ID3D11Buffer> m_pIndexBuffer;                // ����������
	UINT m_VertexStride;                                // �����ֽڴ�С
	UINT m_IndexCount;                                  // ������Ŀ 
};

template<class VertexType, class IndexType>
inline void GameObject::SetBuffer(ID3D11Device* device, const Geometry::MeshData<VertexType, IndexType>& meshData)
{
	/*// �ͷž���Դ
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
	// �ͷž���Դ
	m_pVertexBuffer.Reset();
	m_pIndexBuffer.Reset();

	// ���ö��㻺��������
	m_VertexStride = sizeof(VertexType);
	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = (UINT)meshData.vertexVec.size() * m_VertexStride;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	// �½����㻺����
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = meshData.vertexVec.data();
	HR(device->CreateBuffer(&vbd, &InitData, m_pVertexBuffer.GetAddressOf()));


	// ������������������
	m_IndexCount = (UINT)meshData.indexVec.size();
	D3D11_BUFFER_DESC ibd;
	ZeroMemory(&ibd, sizeof(ibd));
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = m_IndexCount * sizeof(IndexType);
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	// �½�����������
	InitData.pSysMem = meshData.indexVec.data();
	HR(device->CreateBuffer(&ibd, &InitData, m_pIndexBuffer.GetAddressOf()));

}
