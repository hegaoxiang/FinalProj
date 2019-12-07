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

	// ��ȡ����
	static BasicEffect& Get();



	// ��ʼ��Basic.hlsli������Դ����ʼ����Ⱦ״̬
	bool InitAll(ID3D11Device* device);


	//
	// ��Ⱦģʽ�ı��
	//

	// Ĭ��״̬������
	void SetRenderDefault(ID3D11DeviceContext* deviceContext);
	// Alpha��ϻ���
	void SetRenderAlphaBlend(ID3D11DeviceContext* deviceContext);
	// �޶��λ��
	void SetRenderNoDoubleBlend(ID3D11DeviceContext* deviceContext, UINT stencilRef);

	//
	// ��������
	//

	void XM_CALLCONV SetWorldMatrix(DirectX::FXMMATRIX W);
	void XM_CALLCONV SetViewMatrix(DirectX::FXMMATRIX V);
	void XM_CALLCONV SetProjMatrix(DirectX::FXMMATRIX P);


	//
	// ���ա����ʺ������������
	//

	// �������͵ƹ�����������Ŀ
	static const int maxLights = 10;

	void SetDirLight(size_t pos, const DirectionalLight& dirLight);
	void SetPointLight(size_t pos, const PointLight& pointLight);
	void SetSpotLight(size_t pos, const SpotLight& spotLight);

	void SetMaterial(const Material& material);

	void SetTexture(ID3D11ShaderResourceView* texture);

	void XM_CALLCONV SetEyePos(DirectX::FXMVECTOR eyePos);


	// Ӧ�ó�����������������Դ�ı��
	void Apply(ID3D11DeviceContext* deviceContext);

private:
	class Impl;
	std::unique_ptr<Impl> pImpl;
};

