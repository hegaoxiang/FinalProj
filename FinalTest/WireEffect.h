#pragma once
#include "IEffect.h"
#include "Vertex.h"
class WireEffect :
	public IEffect
{
public:
	WireEffect();
	virtual ~WireEffect() override;

	WireEffect(WireEffect&& moveFrom) noexcept;
	WireEffect& operator=(WireEffect&& moveFrom) noexcept;

	// 获取单例
	static WireEffect& Get();



	// 初始化Basic.hlsli所需资源并初始化渲染状态
	bool InitAll(ID3D11Device* device);


	//
	// 渲染模式的变更
	//

	// 默认状态来绘制
	void SetWireRender(ID3D11DeviceContext* deviceContext);
	//
	// 矩阵设置
	//

	void XM_CALLCONV SetViewMatrix(DirectX::FXMMATRIX V);
	void XM_CALLCONV SetProjMatrix(DirectX::FXMMATRIX P);

	// 应用常量缓冲区和纹理资源的变更
	void Apply(ID3D11DeviceContext* deviceContext);

private:
	class Impl;
	std::unique_ptr<Impl> pImpl;
};

