#include "GameObejct.h"
#include "BasicEffect.h"
#include "PostEffect.h"
#include <string>
using namespace std;

GameObject::GameObject()
{
	Init();
}

void GameObject::Init()
{
	m_Components.insert(std::pair<ComponentID, ComponentPtr>(IDModelComponent, new ModelComponent()));
	m_Components[IDModelComponent]->SetRoot(this);
	m_Components.insert(std::pair<ComponentID, ComponentPtr>(IDBasicTextureComponent, new BasicTextureComponent()));
	m_Components[IDBasicTextureComponent]->SetRoot(this);
}

DirectX::BoundingBox GameObject::GetLocalBoundingBox() 
{
	BoundingBox box;
	auto modelComp = dynamic_cast<ModelComponent*>(m_Components[IDModelComponent]);
	
	m_BoundingBox.Transform(box, XMLoadFloat4x4(&modelComp->GetWorldMatrix()));
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
	//box.Transform(box, XMLoadFloat4x4(&m_WorldMatrix));
	return box;
}

DirectX::XMFLOAT3 GameObject::GetPosition() 
{
	auto modelComp = dynamic_cast<ModelComponent*>(m_Components[IDModelComponent]);
	return modelComp->GetPosition();
}

void XM_CALLCONV GameObject::SetWorldMatrix(DirectX::FXMMATRIX world)
{
	auto modelComp = dynamic_cast<ModelComponent*>(m_Components[IDModelComponent]);
	modelComp->SetWorldMatrix(world);
}

void GameObject::SetWorldMatrix(const DirectX::XMFLOAT4X4& world)
{
	auto modelComp = dynamic_cast<ModelComponent*>(m_Components[IDModelComponent]);
	modelComp->SetWorldMatrix(world);
}

void GameObject::SetTexture(ID3D11ShaderResourceView* texture)
{
	auto bcTexComp = dynamic_cast<BasicTextureComponent*>(m_Components[IDBasicTextureComponent]);
	bcTexComp->SetTexture(texture);
}
void GameObject::SetTexture(ID3D11Device* device, const wchar_t* szfileName)
{
	auto bcTexComp = dynamic_cast<BasicTextureComponent*>(m_Components[IDBasicTextureComponent]);
	bcTexComp->SetTexture(device, szfileName);
}


void GameObject::SetMaterial(Material material)
{
	auto bcTexComp = dynamic_cast<BasicTextureComponent*>(m_Components[IDBasicTextureComponent]);
	bcTexComp->SetMaterial(material);
}

void GameObject::Draw(ID3D11DeviceContext* deviceContext, IEffect* effect)
{
	
	auto modelComp = dynamic_cast<ModelComponent*>(m_Components[IDModelComponent]);
	auto bcTexComp = dynamic_cast<BasicTextureComponent*>(m_Components[IDBasicTextureComponent]);

	if (modelComp == nullptr || bcTexComp == nullptr)
		throw "one Component is not exit";
	for (auto item : m_Components)
	{
		item.second->ApplyToDraw(deviceContext, effect);
	}
	effect->Apply(deviceContext);

	// ���Կ�ʼ����
	deviceContext->DrawIndexed(modelComp->GetIndexCount(), 0, 0);
}

void GameObject::SetDebugObjectName(const std::string& name)
{
	//D3D11SetDebugObjectName(m_pVertexBuffer.Get(), "VertexBuffer");
	//D3D11SetDebugObjectName(m_pIndexBuffer.Get(), "IndexBuffer");
}

void GameObject::Serialize(PrettyWriter<StringBuffer>& write)
{
	write.StartObject();

	for (auto& item : m_Components)
	{
		write.Key(to_string(item.second->GetId()).c_str());
		item.second->Serialize(write);
	}
	write.EndObject();
}
