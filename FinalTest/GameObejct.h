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
private:
	DirectX::XMFLOAT4X4 m_WorldMatrix;                  // 世界矩阵
	ComPtr<ID3D11ShaderResourceView> m_pTexture;        // 纹理
	ComPtr<ID3D11Buffer> m_pVertexBuffer;               // 顶点缓冲区
	ComPtr<ID3D11Buffer> m_pIndexBuffer;                // 索引缓冲区
	UINT m_VertexStride;                                // 顶点字节大小
	UINT m_IndexCount;                                  // 索引数目 
};

