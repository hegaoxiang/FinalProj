#pragma once
#include "Component.h"
#include "Vertex.h"
class BasicTextureComponent
	:public Component
{
public:
	BasicTextureComponent() {};
	// ��������
	void SetTexture(ID3D11ShaderResourceView* texture);
	
	void SetTexture(ID3D11Device* device, const wchar_t* szfileName);

	// ���ò���
	void SetMaterial(Material material);

	void ApplyToDraw(ID3D11DeviceContext* deviceContext, IEffect* effect) const override;
private:
	Material m_Material;								// ����
	
	ComPtr<ID3D11ShaderResourceView> m_pTexture;        // ����
};
