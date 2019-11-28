#pragma once
#include<d3d11.h>
#include<DirectXMath.h>
#include <wrl/client.h>
#include"Geometry.h"
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
private:
	DirectX::XMFLOAT4X4 m_WorldMatrix;                  // �������
	ComPtr<ID3D11ShaderResourceView> m_pTexture;        // ����
	ComPtr<ID3D11Buffer> m_pVertexBuffer;               // ���㻺����
	ComPtr<ID3D11Buffer> m_pIndexBuffer;                // ����������
	UINT m_VertexStride;                                // �����ֽڴ�С
	UINT m_IndexCount;                                  // ������Ŀ 
};

