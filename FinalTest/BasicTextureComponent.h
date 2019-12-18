#pragma once
#include "Component.h"
#include "Vertex.h"
class BasicTextureComponent
	:public Component
{
public:
	BasicTextureComponent() : m_textureColor("") { SetId(); };
	// ��������
	void SetTexture(ID3D11ShaderResourceView* texture);
	
	void SetTexture(ID3D11Device* device, const wchar_t* szfileName);
	void SetTexture(ID3D11Device* device, const std::string& fileName);

	// ���ò���
	void SetMaterial(Material material);

	void ApplyToDraw(ID3D11DeviceContext* deviceContext, IEffect* effect) const override;

	void Serialize(PrettyWriter<StringBuffer>& write) override;


	ComponentID GetId() override;


	void AntiSerialize(const char* pData) override;

private:
	void SetId() override;

	Material m_Material;								// ����
	
	ComPtr<ID3D11ShaderResourceView> m_pTexture;        // ����

	// serialize
	std::string m_textureColor;
};

