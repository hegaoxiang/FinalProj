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

	// ��ȡ����
	static WireEffect& Get();



	// ��ʼ��Basic.hlsli������Դ����ʼ����Ⱦ״̬
	bool InitAll(ID3D11Device* device);


	//
	// ��Ⱦģʽ�ı��
	//

	// Ĭ��״̬������
	void SetWireRender(ID3D11DeviceContext* deviceContext);
	//
	// ��������
	//

	void XM_CALLCONV SetViewMatrix(DirectX::FXMMATRIX V);
	void XM_CALLCONV SetProjMatrix(DirectX::FXMMATRIX P);

	// Ӧ�ó�����������������Դ�ı��
	void Apply(ID3D11DeviceContext* deviceContext);

private:
	class Impl;
	std::unique_ptr<Impl> pImpl;
};

