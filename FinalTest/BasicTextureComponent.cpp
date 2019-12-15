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
char* WcharToChar(const wchar_t* wc)
{
	char* m_char = NULL;
	delete m_char;
	m_char = NULL;
	int len = WideCharToMultiByte(CP_ACP, 0, wc, wcslen(wc), NULL, 0, NULL, NULL);
	m_char = new char[len + 1];
	WideCharToMultiByte(CP_ACP, 0, wc, wcslen(wc), m_char, len, NULL, NULL);
	m_char[len] = '\0';
	return m_char;
}
void BasicTextureComponent::SetTexture(ID3D11Device* device, const wchar_t* szfileName)
{
	m_textureColor = WcharToChar(szfileName);

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

void BasicTextureComponent::Serialize(PrettyWriter<StringBuffer>& write)
{
	write.StartObject();

	write.Key("BasicColorTexture");
	write.String(m_textureColor.c_str());

	write.EndObject();
}

void BasicTextureComponent::SetId()
{
	m_id = 2;
}
ComponentID BasicTextureComponent::GetId()
{
	return m_id;
}
