#include "BasicTextureComponent.h"
#include "BasicEffect.h"
#include "PostEffect.h"
void BasicTextureComponent::SetTexture(ID3D11ShaderResourceView* texture)
{
	m_pTexture = texture;
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
