#include "BasicTextureComponent.h"
#include "BasicEffect.h"
#include "PostEffect.h"
#include "DDSTextureLoader.h"
#include "WICTextureLoader.h"
#include "GameObejct.h"
#include <rapidjson/pointer.h>
#include <stdlib.h>

using namespace DirectX;
extern ID3D11Device* g_device;
void BasicTextureComponent::SetTexture(ID3D11ShaderResourceView* texture)
{
	m_pTexture = texture;
}
char* WcharToChar(const wchar_t* wc)
{
	size_t len = wcslen(wc) + 1;
	size_t converted = 0;
	char* CStr;
	CStr = new char[len];
	wcstombs_s(&converted, CStr, len, wc, _TRUNCATE);
	return CStr;
}
wchar_t* CharToWchar(const char* c)
{
	size_t len = strlen(c) + 1;
	size_t converted = 0;
	wchar_t* WStr;
	WStr = new wchar_t[len];
	mbstowcs_s(&converted, WStr, len, c, _TRUNCATE);
	return WStr;
}
void BasicTextureComponent::SetTexture(ID3D11Device* device, const wchar_t* szfileName)
{
	m_textureColor = WcharToChar(szfileName);

	HR(CreateDDSTextureFromFile(device, szfileName, nullptr, m_pTexture.ReleaseAndGetAddressOf()));

	D3D11SetDebugObjectName(m_pTexture.Get(), "BasicTextureComp");
}
void BasicTextureComponent::SetTexture(ID3D11Device* device, const std::string& fileName)
{
	m_textureColor = fileName;

	auto wcStr = CharToWchar(fileName.c_str());
	HR(CreateDDSTextureFromFile(device, wcStr, nullptr, m_pTexture.ReleaseAndGetAddressOf()));

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
	
	write.Key("ComponentTag");
	write.Uint(m_id);

	write.Key("BasicColorTexture");
	write.String(m_textureColor.c_str());

	write.Key("Material");
	write.StartArray();
	float* data = (float*)&m_Material;
	for (int i = 0; i < 12; i++)
	{
		double d =* data;
		write.Double(d);
		data++;
	}
	write.EndArray();

	write.EndObject();
}

void BasicTextureComponent::SetId()
{
	m_id = IDBasicTextureComponent;
}
ComponentID BasicTextureComponent::GetId()
{
	return m_id;
}

void BasicTextureComponent::AntiSerialize(const char* pData)
{
	Document d;
	d.Parse(pData);

	auto path = Pointer("/BasicColorTexture").Get(d);
	SetTexture(g_device, path->GetString());

	auto materialValues = Pointer("/Material").Get(d);

	float* data = (float*)&m_Material;
	for (auto& item:materialValues->GetArray())
	{
		*data = item.GetDouble();
		data++;
	}

}
