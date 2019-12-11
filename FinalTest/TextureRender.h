#pragma once
#include <d3d11.h>
#include <string>
#include <wrl/client.h>

class TextureRender
{
public:
	template<class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;


	TextureRender(ID3D11Device* device,
		int texWidth,
		int texHeight,
		bool generateMips = false);
	~TextureRender();

	// ��ʼ�Ե�ǰ���������Ⱦ
	void Begin(ID3D11DeviceContext* deviceContext);
	// �����Ե�ǰ�������Ⱦ����ԭ״̬
	void End(ID3D11DeviceContext* deviceContext);
	// ��ȡ��Ⱦ�õ�����
	ID3D11ShaderResourceView* GetOutputTexture();

	// ���õ��Զ�����
	void SetDebugObjectName(const std::string& name);

private:
	ComPtr<ID3D11ShaderResourceView>    m_pOutputTextureSRV;    // ����������Ӧ����ɫ����Դ��ͼ
	ComPtr<ID3D11RenderTargetView>      m_pOutputTextureRTV;    // ����������Ӧ����ȾĿ����ͼ
	ComPtr<ID3D11DepthStencilView>      m_pOutputTextureDSV;    // ����������õ����/ģ����ͼ
	D3D11_VIEWPORT                      m_OutputViewPort;       // ������õ��ӿ�

	ComPtr<ID3D11RenderTargetView>      m_pCacheRTV;            // ��ʱ����ĺ󱸻�����
	ComPtr<ID3D11DepthStencilView>      m_pCacheDSV;            // ��ʱ��������/ģ�建����
	D3D11_VIEWPORT                      m_CacheViewPort;        // ��ʱ������ӿ�

	bool                                m_GenerateMips;         // �Ƿ�����mipmap��
};

