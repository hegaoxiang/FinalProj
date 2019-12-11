#pragma once
#include "IEffect.h"
#include "Vertex.h"
class PostEffect :
	public IEffect
{
public:
	PostEffect();
	virtual ~PostEffect() override;

	PostEffect(PostEffect&& moveFrom) noexcept;
	PostEffect& operator=(PostEffect&& moveFrom) noexcept;

	// 获取单例
	static PostEffect& Get();



	// 初始化Basic.hlsli所需资源并初始化渲染状态
	bool InitAll(ID3D11Device* device);


	//
	// 渲染模式的变更
	//

	// 默认状态来绘制
	void SetRenderDefault(ID3D11DeviceContext* deviceContext);
	

	//
	// 纹理设置
	//

	void SetTexture(ID3D11ShaderResourceView* texture);



	// 应用常量缓冲区和纹理资源的变更
	void Apply(ID3D11DeviceContext* deviceContext);

private:
	class Impl;
	std::unique_ptr<Impl> pImpl;
};

