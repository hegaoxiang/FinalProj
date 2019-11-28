#include "GameObejct.h"
#include "DXTrace.h"
template<class VertexType, class IndexType>
void GameObject::SetBuffer(ID3D11Device* device, const Geometry::MeshData<VertexType, IndexType>& meshData)
{
	// 释放旧资源
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

	HR(device->CreateBuffer(&bd, &initData, m_pIndexBuffer.GetAddressOf()));
}

GameObject::GameObject()
	: m_IndexCount(), m_VertexStride()
{
	XMStoreFloat4x4(&m_WorldMatrix, DirectX::XMMatrixIdentity());
}
DirectX::XMFLOAT3 GameObject::GetPosition() const
{
	return DirectX::XMFLOAT3(m_WorldMatrix(3, 0), m_WorldMatrix(3, 1), m_WorldMatrix(3, 2));
}

void GameObject::SetTexture(ID3D11ShaderResourceView* texture)
{
	m_pTexture = texture;
}

void GameObject::SetWorldMatrix(const DirectX::XMFLOAT4X4& world)
{
	m_WorldMatrix = world;
}

void XM_CALLCONV GameObject::SetWorldMatrix(DirectX::FXMMATRIX world)
{
	DirectX::XMStoreFloat4x4(&m_WorldMatrix, world);
}

void GameObject::Draw(ID3D11DeviceContext* deviceContext)
{
	UINT offset = 0;
	deviceContext->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), &m_VertexStride, &offset);
	
	deviceContext->IASetIndexBuffer(m_pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

	// 获取之前已经绑定到渲染管线上的常量缓冲区并进行修改
	ComPtr<ID3D11Buffer> cBuffer = nullptr;
	deviceContext->VSGetConstantBuffers(0, 1, cBuffer.GetAddressOf());
	CBChangesEveryDrawing cbDrawing;

	// 内部进行转置，这样外部就不需要提前转置了
	XMMATRIX W = XMLoadFloat4x4(&m_WorldMatrix);
	cbDrawing.world = XMMatrixTranspose(W);
	

	// 更新常量缓冲区
	D3D11_MAPPED_SUBRESOURCE mappedData;
	HR(deviceContext->Map(cBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));
	memcpy_s(mappedData.pData, sizeof(CBChangesEveryDrawing), &cbDrawing, sizeof(CBChangesEveryDrawing));
	deviceContext->Unmap(cBuffer.Get(), 0);
	// 设置纹理
	deviceContext->PSSetShaderResources(0, 1, m_pTexture.GetAddressOf());
	// 可以开始绘制
	deviceContext->DrawIndexed(m_IndexCount, 0, 0);
}

void GameObject::SetDebugObjectName(const std::string& name)
{
	//D3D11SetDebugObjectName(m_pVertexBuffer.Get(), "VertexBuffer");
	//D3D11SetDebugObjectName(m_pIndexBuffer.Get(), "IndexBuffer");
}
