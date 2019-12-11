#include "GameObejct.h"
#include "BasicEffect.h"
#include "PostEffect.h"


GameObject::GameObject()
	: m_IndexCount(), m_VertexStride()
{
	XMStoreFloat4x4(&m_WorldMatrix, DirectX::XMMatrixIdentity());
}
DirectX::XMFLOAT3 GameObject::GetPosition() const
{
	return DirectX::XMFLOAT3(m_WorldMatrix(3, 0), m_WorldMatrix(3, 1), m_WorldMatrix(3, 2));
}

DirectX::BoundingBox GameObject::GetLocalBoundingBox() const
{
	BoundingBox box;
	m_BoundingBox.Transform(box, XMLoadFloat4x4(&m_WorldMatrix));
	return box;
}

DirectX::BoundingBox GameObject::GetBoundingBox() const
{
	return m_BoundingBox;
}

DirectX::BoundingOrientedBox GameObject::GetBoundingOrientedBox() const
{
	BoundingOrientedBox box;
	BoundingOrientedBox::CreateFromBoundingBox(box, m_BoundingBox);
	box.Transform(box, XMLoadFloat4x4(&m_WorldMatrix));
	return box;
}

void GameObject::SetTexture(ID3D11ShaderResourceView* texture)
{
	m_pTexture = texture;
}

void GameObject::SetWorldMatrix(const DirectX::XMFLOAT4X4& world)
{
	m_WorldMatrix = world;
}

void GameObject::SetMaterial(Material material)
{
	m_Material = material;
}

void XM_CALLCONV GameObject::SetWorldMatrix(DirectX::FXMMATRIX world)
{
	DirectX::XMStoreFloat4x4(&m_WorldMatrix, world);
}

void GameObject::Draw(ID3D11DeviceContext* deviceContext, IEffect* effect)
{
	auto basicEffect = dynamic_cast<BasicEffect*>(effect);
	auto postEffect = dynamic_cast<PostEffect*>(effect);
	UINT offset = 0;
	deviceContext->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), &m_VertexStride, &offset);
	deviceContext->IASetIndexBuffer(m_pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

	if (basicEffect)
	{
		basicEffect->SetWorldMatrix(XMLoadFloat4x4(&m_WorldMatrix));
		basicEffect->SetMaterial(m_Material);
		basicEffect->SetTexture(m_pTexture.Get());
		basicEffect->Apply(deviceContext);
	}
	else if (postEffect)
	{
		postEffect->SetTexture(m_pTexture.Get());
		postEffect->Apply(deviceContext);
	}

	// 可以开始绘制
	deviceContext->DrawIndexed(m_IndexCount, 0, 0);
}

void GameObject::SetDebugObjectName(const std::string& name)
{
	//D3D11SetDebugObjectName(m_pVertexBuffer.Get(), "VertexBuffer");
	//D3D11SetDebugObjectName(m_pIndexBuffer.Get(), "IndexBuffer");
}
