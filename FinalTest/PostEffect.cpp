#include "PostEffect.h"

#include "CBuffer.h"
#include <d3dcompiler.h>
#include <vector>
#include "RenderStates.h"
#include "DXTrace.h"
namespace
{
	// PostEffect单例
	static PostEffect* g_pInstance = new PostEffect();
}

class PostEffect::Impl
{

public:
	ComPtr<ID3D11VertexShader> m_pVertexShader3D;               // 用于3D的顶点着色器
	ComPtr<ID3D11PixelShader>  m_pPixelShader3D;
	ComPtr<ID3D11InputLayout>  m_pVertexLayout3D;

	ComPtr<ID3D11ShaderResourceView> m_pTexture;
};
PostEffect::~PostEffect()
{
	delete g_pInstance;
}

PostEffect::PostEffect()
{
	if (g_pInstance)
		throw std::exception("BasicEffect is a singleton!");
	g_pInstance = this;
	pImpl = std::make_unique<PostEffect::Impl>();
}

PostEffect::PostEffect(PostEffect&& moveFrom) noexcept
{
	pImpl.swap(moveFrom.pImpl);
}

PostEffect& PostEffect::Get()
{
	return *g_pInstance;
}

bool PostEffect::InitAll(ID3D11Device* device)
{
	if (!device)
		return false;
	if (!RenderStates::IsInit())
	{
		RenderStates::InitAll(device);
		//throw std::exception("RenderStates need to be initialized first!");
	}

	ComPtr<ID3DBlob> blob;

	// 创建顶点着色器(3D)
	HR(CreateShaderFromFile(L"HLSL\\PostVS.cso", L"HLSL\\PostVS.hlsl", "VS", "vs_5_0", blob.ReleaseAndGetAddressOf()));
	HR(device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, pImpl->m_pVertexShader3D.GetAddressOf()));
	// 创建顶点布局(3D)
	HR(device->CreateInputLayout(VertexPosTex::inputLayout, ARRAYSIZE(VertexPosTex::inputLayout),
		blob->GetBufferPointer(), blob->GetBufferSize(), pImpl->m_pVertexLayout3D.GetAddressOf()));

	// 创建像素着色器(3D)
	HR(CreateShaderFromFile(L"HLSL\\PostPS.cso", L"HLSL\\PostPS.hlsl", "PS", "ps_5_0", blob.ReleaseAndGetAddressOf()));
	HR(device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, pImpl->m_pPixelShader3D.GetAddressOf()));

	return true;
}

void PostEffect::SetRenderDefault(ID3D11DeviceContext* deviceContext)
{
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	deviceContext->IASetInputLayout(pImpl->m_pVertexLayout3D.Get());
	deviceContext->VSSetShader(pImpl->m_pVertexShader3D.Get(), nullptr, 0);
	deviceContext->RSSetState(nullptr);
	deviceContext->PSSetShader(pImpl->m_pPixelShader3D.Get(), nullptr, 0);
	deviceContext->PSSetSamplers(0, 1, RenderStates::SSLinearWrap.GetAddressOf());
	deviceContext->OMSetDepthStencilState(nullptr, 0);
	deviceContext->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);
}

void PostEffect::SetTexture(ID3D11ShaderResourceView* texture)
{
	pImpl->m_pTexture = texture;
}

void PostEffect::Apply(ID3D11DeviceContext* deviceContext)
{
	// 设置纹理
	deviceContext->PSSetShaderResources(0, 1, pImpl->m_pTexture.GetAddressOf());

}

PostEffect& PostEffect::operator=(PostEffect&& moveFrom) noexcept
{
	pImpl.swap(moveFrom.pImpl);
	return *this;
}