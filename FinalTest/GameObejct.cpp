#include "GameObejct.h"



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

	// ��ȡ֮ǰ�Ѿ��󶨵���Ⱦ�����ϵĳ����������������޸�
	ComPtr<ID3D11Buffer> cBuffer = nullptr;
	deviceContext->VSGetConstantBuffers(0, 1, cBuffer.GetAddressOf());
	CBChangesEveryDrawing cbDrawing;

	// �ڲ�����ת�ã������ⲿ�Ͳ���Ҫ��ǰת����
	XMMATRIX W = XMLoadFloat4x4(&m_WorldMatrix);
	cbDrawing.world = XMMatrixTranspose(W);
	

	// ���³���������
	D3D11_MAPPED_SUBRESOURCE mappedData;
	HR(deviceContext->Map(cBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));
	memcpy_s(mappedData.pData, sizeof(CBChangesEveryDrawing), &cbDrawing, sizeof(CBChangesEveryDrawing));
	deviceContext->Unmap(cBuffer.Get(), 0);
	// ��������
	deviceContext->PSSetShaderResources(0, 1, m_pTexture.GetAddressOf());
	// ���Կ�ʼ����
	deviceContext->DrawIndexed(m_IndexCount, 0, 0);
}

void GameObject::SetDebugObjectName(const std::string& name)
{
	//D3D11SetDebugObjectName(m_pVertexBuffer.Get(), "VertexBuffer");
	//D3D11SetDebugObjectName(m_pIndexBuffer.Get(), "IndexBuffer");
}