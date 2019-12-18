#include "ModelComponent.h"
#include "BasicEffect.h"
#include "GameObejct.h"
#include <rapidjson\document.h>
#include <rapidjson\pointer.h>

extern ID3D11Device* g_device;
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

void ModelComponent::SetPosition(const DirectX::XMFLOAT3& position)
{
	m_Position = position;

	m_WorldMatrix._41 = position.x;
	m_WorldMatrix._42 = position.y;
	m_WorldMatrix._43 = position.z;
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

	write.Key("ComponentTag");
	write.Uint(m_id);

	write.Key("transform");
	write.StartArray();

	float* data = (float*)m_WorldMatrix.m;
	for (int i = 0; i < 16; i++)
	{
		write.Double(*data);
		data++;
	}
	write.EndArray();
	#if 0
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
#endif


	write.Key("Mesh");

	write.StartObject();
	#pragma region Mesh	

	write.Key("isOutside");
	write.Bool(m_isOutside);
	
	write.Key("VertexType");
	//write.Int(m_Vertextype);
	write.Int(1);
	
	if (true)
	{
		write.Key("Primitive");
		//write.Int(m_Primitive);
		
		write.Int(1);
	
		write.Key("Detail");
		write.StartArray();
		for (auto item: m_MeshAttrs)
		{
			write.Double(item);
		}
		write.EndArray();
	}
	#pragma endregion 
	write.EndObject();

	write.EndObject();
}


void ModelComponent::SetBufferAttr(int vertexType, int primitive, std::initializer_list<float>details)
{
	m_Primitive = primitive;
	m_Vertextype = vertexType;

	for (auto item : details)
	{
		m_MeshAttrs.push_back(item);
	}
}

void ModelComponent::SetId()
{
	m_id = IDModelComponent;
}

ComponentID ModelComponent::GetId()
{
	return m_id;
}

void ModelComponent::AntiSerialize(const char* pData)
{
	assert(pData);

	Document d;
	d.Parse(pData);

	XMFLOAT4X4 worldMat;
	float* data = (float*)(worldMat.m);
	auto transform = Pointer("/transform").Get(d);
	for (auto& item : transform->GetArray())
	{
		*data = item.GetDouble();
		data++;
	}
	SetWorldMatrix(worldMat);

	if (Pointer("/Mesh/VertexType").Get(d)->GetInt() == 1 && Pointer("/Mesh/Primitive").Get(d)->GetInt() == 1)
	{
		SetBuffer(g_device, Geometry::CreatePlane<VertexPosNormalTex>(
			d["Mesh"]["Detail"][0].GetDouble(),
			d["Mesh"]["Detail"][1].GetDouble(),
			d["Mesh"]["Detail"][2].GetDouble(),
			d["Mesh"]["Detail"][3].GetDouble(),
			d["Mesh"]["Detail"][4].GetDouble(),
			d["Mesh"]["Detail"][5].GetDouble(),
			d["Mesh"]["Detail"][6].GetDouble()
			));
	}
	else if (Pointer("/Mesh/VertexType").Get(d)->GetInt() == 1 && Pointer("/Mesh/Primitive").Get(d)->GetInt() == 2)
		SetBuffer(g_device, Geometry::CreatePlane<VertexPosNormalTex>());
	else
		;
}
