#include "ModelComponent.h"
#include "BasicEffect.h"

DirectX::XMFLOAT4X4 ModelComponent::GetWorldMatrix() const
{
	return m_WorldMatrix;
}

DirectX::XMFLOAT3 ModelComponent::GetPosition() const
{
	return m_Position;
}

void XM_CALLCONV ModelComponent::SetWorldMatrix(DirectX::FXMMATRIX world)
{
	DirectX::XMStoreFloat4x4(&m_WorldMatrix, world);
	m_Position = DirectX::XMFLOAT3(m_WorldMatrix(3, 0), m_WorldMatrix(3, 1), m_WorldMatrix(3, 2));
}

void ModelComponent::SetWorldMatrix(const DirectX::XMFLOAT4X4& world)
{
	m_WorldMatrix = world;
	m_Position = DirectX::XMFLOAT3(m_WorldMatrix(3, 0), m_WorldMatrix(3, 1), m_WorldMatrix(3, 2));
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

void ModelComponent::Serialize(PrettyWriter<StringBuffer>& write)
{
	write.StartObject();
	
	write.Key("transform");
	write.StartObject();
	#pragma region transform
    write.Key("position");
    write.StartObject();
	#pragma region position

	write.Key("x");
	write.Double(m_Position.x);

	write.Key("y");
	write.Double(m_Position.y);

	write.Key("z");
	write.Double(m_Position.z);

#pragma endregion 
	write.EndObject();
#pragma endregion
	write.EndObject();

	write.Key("Mesh");
	write.StartObject();
	#pragma region Mesh	
	write.Key("isOutside");
	write.Bool(m_isOutside);
	write.Key("VertexType");
	write.Int(m_Vertextype); 
	write.Key("Primitive");
	write.Int(m_Primitive);
	#pragma endregion 
	write.EndObject();

	write.EndObject();
}

void ModelComponent::SetBufferAttr(int vertexType, int primitive)
{
	m_Primitive = primitive;
	m_Vertextype = vertexType;
}

void ModelComponent::SetId()
{
	m_id = 1;
}

ComponentID ModelComponent::GetId()
{
	return m_id;
}

