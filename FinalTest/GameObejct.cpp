#include "GameObejct.h"
#include "BasicEffect.h"
#include "PostEffect.h"
#include <string>
using namespace std;

ActorId autoId = 0;

GameObject::GameObject()
{
	if (false)
		Init();
}

void GameObject::Init()
{
	m_Components.insert(std::pair<ComponentID, StrongCompnentPtr>(IDModelComponent,make_shared<ModelComponent>()));
	m_Components[IDModelComponent]->SetRoot(this);
	m_Components.insert(std::pair<ComponentID, StrongCompnentPtr>(IDBasicTextureComponent, make_shared<BasicTextureComponent>()));
	m_Components[IDBasicTextureComponent]->SetRoot(this);

	m_name = "default";
}

DirectX::BoundingBox GameObject::GetLocalBoundingBox() 
{
	BoundingBox box;
	auto modelComp = dynamic_cast<ModelComponent*>(m_Components[IDModelComponent].get());
	
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
	auto modelComp = dynamic_cast<ModelComponent*>(m_Components[IDModelComponent].get());
	return modelComp->GetPosition();
}

void XM_CALLCONV GameObject::SetWorldMatrix(DirectX::FXMMATRIX world)
{
	auto modelComp = dynamic_cast<ModelComponent*>(m_Components[IDModelComponent].get());
	modelComp->SetWorldMatrix(world);
}

void GameObject::SetWorldMatrix(const DirectX::XMFLOAT4X4& world)
{
	auto modelComp = dynamic_cast<ModelComponent*>(m_Components[IDModelComponent].get());
	modelComp->SetWorldMatrix(world);
}

void GameObject::SetTexture(ID3D11ShaderResourceView* texture)
{
	auto bcTexComp = dynamic_cast<BasicTextureComponent*>(m_Components[IDBasicTextureComponent].get());
	bcTexComp->SetTexture(texture);
}
void GameObject::SetTexture(ID3D11Device* device, const wchar_t* szfileName)
{
	auto bcTexComp = dynamic_cast<BasicTextureComponent*>(m_Components[IDBasicTextureComponent].get());
	bcTexComp->SetTexture(device, szfileName);
}


void GameObject::SetMaterial(Material material)
{
	auto bcTexComp = dynamic_cast<BasicTextureComponent*>(m_Components[IDBasicTextureComponent].get());
	bcTexComp->SetMaterial(material);
}

void GameObject::Draw(ID3D11DeviceContext* deviceContext, IEffect* effect)
{
	
	auto modelComp = dynamic_cast<ModelComponent*>(m_Components[IDModelComponent].get());
	auto bcTexComp = dynamic_cast<BasicTextureComponent*>(m_Components[IDBasicTextureComponent].get());

	if (modelComp == nullptr || bcTexComp == nullptr)
		throw "one Component is not exit";
	for (auto item : m_Components)
	{
		item.second->ApplyToDraw(deviceContext, effect);
	}
	effect->Apply(deviceContext);

	// 可以开始绘制
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

	write.Key("name");
	write.String(m_name.c_str());

	write.Key("ComponentsAttribute");
	write.StartArray();
	for (auto& item : m_Components)
	{
		item.second->Serialize(write);
	}
	write.EndArray();

	write.EndObject();
}

void GameObject::SetBufferAttr(int vertexType, int primitive, std::initializer_list<float>details)
{
	auto modelComp = dynamic_cast<ModelComponent*>(m_Components[IDModelComponent].get());
	modelComp->SetBufferAttr(vertexType,primitive,details);
}

void GameObject::AddCompent(Component* pComp)
{
	pComp->SetRoot(this);
	m_Components.insert(std::pair<ComponentID, StrongCompnentPtr>(pComp->GetId(), pComp));
}
