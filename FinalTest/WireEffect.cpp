#include "WireEffect.h"

#include "DXTrace.h"
namespace
{
	static WireEffect* g_pInstance = new WireEffect();
}

class WireEffect::Impl :public AlignedType<WireEffect::Impl>
{
public:
	struct CBChangesEveryFrame
	{
		DirectX::XMMATRIX view;
	};
	struct CBChangesOnResize
	{
		DirectX::XMMATRIX proj;
	};

public:
	// 必须显式指定
	Impl() : m_IsDirty() {}
	~Impl() = default;

public:
	CBufferObject<0, CBChangesEveryFrame> m_CBFrame;
	CBufferObject<1, CBChangesOnResize> m_CBResize;
	bool m_IsDirty;

	std::vector<CBufferBase*> m_pCBuffers; // 管理上面的CBufferObject

	ComPtr<ID3D11VertexShader> m_pVertexShader3D;               // 用于3D的顶点着色器
	ComPtr<ID3D11PixelShader>  m_pPixelShader3D;                // 用于3D的像素着色器

	ComPtr<ID3D11InputLayout>  m_pVertexLayout3D;               // 用于3D的顶点输入布局

};
WireEffect::~WireEffect()
{
	delete g_pInstance;
}

WireEffect::WireEffect()
{
	if (g_pInstance)
		throw std::exception("WireEffect is a singleton!");
	g_pInstance = this;
	pImpl = std::make_unique<WireEffect::Impl>();
}

WireEffect::WireEffect(WireEffect&& moveFrom) noexcept
{
	pImpl.swap(moveFrom.pImpl);
}

WireEffect& WireEffect::Get()
{
	return *g_pInstance;
}

bool WireEffect::InitAll(ID3D11Device* device)
{
	if (!device)
		return false;

	if (!pImpl->m_pCBuffers.empty())
		return true;

	if (!RenderStates::IsInit())
		RenderStates::InitAll(device);

	ComPtr<ID3DBlob> blob;

	// 创建顶点着色器(3D)
	HR(CreateShaderFromFile(L"HLSL\\WireFrameVS.cso", L"HLSL\\WireFrameVS.hlsl", "VS", "vs_5_0", blob.ReleaseAndGetAddressOf()));
	HR(device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, pImpl->m_pVertexShader3D.GetAddressOf()));
	// 创建顶点布局(3D)
	HR(device->CreateInputLayout(VertexPosColor::inputLayout, ARRAYSIZE(VertexPosColor::inputLayout),
		blob->GetBufferPointer(), blob->GetBufferSize(), pImpl->m_pVertexLayout3D.GetAddressOf()));

	// 创建像素着色器(3D)
	HR(CreateShaderFromFile(L"HLSL\\WireFramePS.cso", L"HLSL\\WireFramePS.hlsl", "PS", "ps_5_0", blob.ReleaseAndGetAddressOf()));
	HR(device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, pImpl->m_pPixelShader3D.GetAddressOf()));

	pImpl->m_pCBuffers.assign({
		&pImpl->m_CBFrame,
		&pImpl->m_CBResize });

	for (auto& buffer : pImpl->m_pCBuffers)
	{
		HR(buffer->CreateBuffer(device));
	}

	return true;
}

void WireEffect::SetWireRender(ID3D11DeviceContext* deviceContext)
{
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	deviceContext->IASetInputLayout(pImpl->m_pVertexLayout3D.Get());
	deviceContext->VSSetShader(pImpl->m_pVertexShader3D.Get(), nullptr, 0);
	deviceContext->RSSetState(RenderStates::RSWireframe.Get());
	deviceContext->PSSetShader(pImpl->m_pPixelShader3D.Get(), nullptr, 0);
	deviceContext->OMSetDepthStencilState(nullptr, 0);
	deviceContext->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);
}

void XM_CALLCONV WireEffect::SetViewMatrix(DirectX::FXMMATRIX V)
{
	auto& cBuffer = pImpl->m_CBFrame;
	cBuffer.data.view = XMMatrixTranspose(V);
	pImpl->m_IsDirty = cBuffer.isDirty = true;
}

void XM_CALLCONV WireEffect::SetProjMatrix(DirectX::FXMMATRIX P)
{
	auto& cBuffer = pImpl->m_CBResize;
	cBuffer.data.proj = XMMatrixTranspose(P);
	pImpl->m_IsDirty = cBuffer.isDirty = true;
}

void WireEffect::Apply(ID3D11DeviceContext* deviceContext)
{
	auto& pCBuffers = pImpl->m_pCBuffers;
	// 将缓冲区绑定到渲染管线上
	pCBuffers[0]->BindVS(deviceContext);
	pCBuffers[1]->BindVS(deviceContext);

	if (pImpl->m_IsDirty)
	{
		pImpl->m_IsDirty = false;
		for (auto& cbuffer : pCBuffers)
		{
			cbuffer->UpdateBuffer(deviceContext);
		}
	}
}

WireEffect& WireEffect::operator=(WireEffect&& moveFrom) noexcept
{
	pImpl.swap(moveFrom.pImpl);
	return *this;
}
