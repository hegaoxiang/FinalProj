#include "GameObejct.h"
#include "BasicEffect.h"
#include "PostEffect.h"

emu
{

}
GameObject::GameObject()
{
	Init();
}

void GameObject::Init()
{
	m_Components.insert(std::pair<ComponentID, ComponentPtr>("ModelComponent", new ModelComponent()));
	m_Components["ModelComponent"]->SetRoot(this);
	m_Components.insert(std::pair<ComponentID, ComponentPtr>("BasicTextureComponent", new BasicTextureComponent()));
	m_Components["BasicTextureComponent"]->SetRoot(this);
}

DirectX::BoundingBox GameObject::GetLocalBoundingBox() const
{
	BoundingBox box;
	//m_BoundingBox.Transform(box, XMLoadFloat4x4(&m_Components["ModelComponent"]));
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
	auto modelComp = dynamic_cast<ModelComponent*>(m_Components["ModelComponent"]);
	return modelComp->GetPosition();
}

void XM_CALLCONV GameObject::SetWorldMatrix(DirectX::FXMMATRIX world)
{
	auto modelComp = dynamic_cast<ModelComponent*>(m_Components["ModelComponent"]);
	modelComp->SetWorldMatrix(world);
}

void GameObject::SetWorldMatrix(const DirectX::XMFLOAT4X4& world)
{
	auto modelComp = dynamic_cast<ModelComponent*>(m_Components["ModelComponent"]);
	modelComp->SetWorldMatrix(world);
}

void GameObject::SetTexture(ID3D11ShaderResourceView* texture)
{
	auto bcTexComp = dynamic_cast<BasicTextureComponent*>(m_Components["BasicTextureComponent"]);
	bcTexComp->SetTexture(texture);
}

void GameObject::SetMaterial(Material material)
{
	auto bcTexComp = dynamic_cast<BasicTextureComponent*>(m_Components["BasicTextureComponent"]);
	bcTexComp->SetMaterial(material);
}

void GameObject::Draw(ID3D11DeviceContext* deviceContext, IEffect* effect)
{
	
	auto modelComp = dynamic_cast<ModelComponent*>(m_Components["ModelComponent"]);
	auto bcTexComp = dynamic_cast<BasicTextureComponent*>(m_Components["BasicTextureComponent"]);

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
