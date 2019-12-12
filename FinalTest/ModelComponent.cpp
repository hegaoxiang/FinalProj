#include "ModelComponent.h"
#include "BasicEffect.h"

DirectX::XMFLOAT3 ModelComponent::GetPosition() const
{
	return DirectX::XMFLOAT3(m_WorldMatrix(3, 0), m_WorldMatrix(3, 1), m_WorldMatrix(3, 2));
}

void XM_CALLCONV ModelComponent::SetWorldMatrix(DirectX::FXMMATRIX world)
{
	DirectX::XMStoreFloat4x4(&m_WorldMatrix, world);
}

void ModelComponent::SetWorldMatrix(const DirectX::XMFLOAT4X4& world)
{
	m_WorldMatrix = world;
}

void ModelComponent::ApplyToDraw(ID3D11DeviceContext* deviceContext, IEffect* effect) const
{
	UINT offset = 0;
	deviceContext->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), &m_VertexStride, &offset);
	deviceContext->IASetIndexBuffer(m_pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

	auto basicEffect = dynamic_cast<BasicEffect*>(effect);

	if (basicEffect)
	{
		basicEffect->SetWorldMatrix(XMLoadFloat4x4(&m_WorldMatrix));
	}

}

UINT ModelComponent::GetIndexCount() const
{
	return m_IndexCount;
}

