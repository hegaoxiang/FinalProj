#include "BasicTextureComponent.h"
#include "BasicEffect.h"
#include "PostEffect.h"
#include "DDSTextureLoader.h"
#include "WICTextureLoader.h"

using namespace DirectX;
void BasicTextureComponent::SetTexture(ID3D11ShaderResourceView* texture)
{
	m_pTexture = texture;
}

void BasicTextureComponent::SetTexture(ID3D11Device* device, const wchar_t* szfileName)
{
	HR(CreateDDSTextureFromFile(device, szfileName, nullptr, m_pTexture.ReleaseAndGetAddressOf()));

	D3D11SetDebugObjectName(m_pTexture.Get(), "BasicTextureComp");
}
void BasicTextureComponent::SetMaterial(Material material)
{
	m_Material = material;
}

void BasicTextureComponent::ApplyToDraw(ID3D11DeviceContext* deviceContext, IEffect* effect) const
{
	auto basicEffect = dynamic_cast<BasicEffect*>(effect);
	auto postEffect = dynamic_cast<PostEffect*>(effect);

	if (basicEffect)
	{
		basicEffect->SetMaterial(m_Material);
		basicEffect->SetTexture(m_pTexture.Get());
	}
	else if (postEffect)
	{
		postEffect->SetTexture(m_pTexture.Get());
	}
}
