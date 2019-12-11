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

	// ��ȡ����
	static PostEffect& Get();



	// ��ʼ��Basic.hlsli������Դ����ʼ����Ⱦ״̬
	bool InitAll(ID3D11Device* device);


	//
	// ��Ⱦģʽ�ı��
	//

	// Ĭ��״̬������
	void SetRenderDefault(ID3D11DeviceContext* deviceContext);
	

	//
	// ��������
	//

	void SetTexture(ID3D11ShaderResourceView* texture);



	// Ӧ�ó�����������������Դ�ı��
	void Apply(ID3D11DeviceContext* deviceContext);

private:
	class Impl;
	std::unique_ptr<Impl> pImpl;
};

