#include "GameObejct.h"
#include "BasicEffect.h"
#include "PostEffect.h"


GameObject::GameObject()
{
	Init();
}

void GameObject::Init()
{
	m_Components.insert(std::pair<const std::string&, ComponentPtr>("ModelComponent", new ModelComponent()));
	m_Components["ModelComponent"]->SetRoot(this);
	m_Components.insert(std::pair<const std::string&, ComponentPtr>("BasicTextureComponent", new BasicTextureComponent()));
	m_Components["BasicTextureComponent"]->SetRoot(this);
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
	auto basicEffect = dynamic_cast<BasicEffect*>(effect);
	auto postEffect = dynamic_cast<PostEffect*>(effect);

	if (basicEffect)
	{
		
	}

	// 可以开始绘制
	deviceContext->DrawIndexed(, 0, 0);
}

void GameObject::SetDebugObjectName(const std::string& name)
{
	//D3D11SetDebugObjectName(m_pVertexBuffer.Get(), "VertexBuffer");
	//D3D11SetDebugObjectName(m_pIndexBuffer.Get(), "IndexBuffer");
}
