#pragma once
#include "Component.h"
#include "Vertex.h"
class BasicTextureComponent
	:public Component
{
public:
	BasicTextureComponent() {};
	// 设置纹理
	void SetTexture(ID3D11ShaderResourceView* texture);
	
	// 设置材质
	void SetMaterial(Material material);

	void ApplyToDraw(ID3D11DeviceContext* deviceContext, IEffect* effect) const override;
private:
	Material m_Material;								// 材质
	
	ComPtr<ID3D11ShaderResourceView> m_pTexture;        // 纹理
};

