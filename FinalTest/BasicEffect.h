#pragma once
#include "IEffect.h"
#include "Vertex.h"
class BasicEffect :
	public IEffect
{
public:

	BasicEffect();
	virtual ~BasicEffect() override;

	BasicEffect(BasicEffect&& moveFrom) noexcept;
	BasicEffect& operator=(BasicEffect&& moveFrom) noexcept;

	// 获取单例
	static BasicEffect& Get();



	// 初始化Basic.hlsli所需资源并初始化渲染状态
	bool InitAll(ID3D11Device* device);


	//
	// 渲染模式的变更
	//

	// 默认状态来绘制
	void SetRenderDefault(ID3D11DeviceContext* deviceContext);
	// Alpha混合绘制
	void SetRenderAlphaBlend(ID3D11DeviceContext* deviceContext);
	// 无二次混合
	void SetRenderNoDoubleBlend(ID3D11DeviceContext* deviceContext, UINT stencilRef);

	//
	// 矩阵设置
	//

	void XM_CALLCONV SetWorldMatrix(DirectX::FXMMATRIX W);
	void XM_CALLCONV SetViewMatrix(DirectX::FXMMATRIX V);
	void XM_CALLCONV SetProjMatrix(DirectX::FXMMATRIX P);


	//
	// 光照、材质和纹理相关设置
	//

	// 各种类型灯光允许的最大数目
	static const int maxLights = 10;

	void SetDirLight(size_t pos, const DirectionalLight& dirLight);
	void SetPointLight(size_t pos, const PointLight& pointLight);
	void SetSpotLight(size_t pos, const SpotLight& spotLight);

	void SetMaterial(const Material& material);

	void SetTexture(ID3D11ShaderResourceView* texture);

	void XM_CALLCONV SetEyePos(DirectX::FXMVECTOR eyePos);


	// 应用常量缓冲区和纹理资源的变更
	void Apply(ID3D11DeviceContext* deviceContext);

private:
	class Impl;
	std::unique_ptr<Impl> pImpl;
};

