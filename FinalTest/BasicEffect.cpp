#include "BasicEffect.h"

#include "CBuffer.h"
#include <d3dcompiler.h>
#include <vector>
#include "RenderStates.h"
#include "DXTrace.h"
using namespace DirectX;

class BasicEffect::Impl : public AlignedType<BasicEffect::Impl>
{
public:

	//
	// ��Щ�ṹ���ӦHLSL�Ľṹ�塣��Ҫ��16�ֽڶ���
	//

	struct CBChangesEveryDrawing
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX worldInvTranspose;
		Material material;
	};


	struct CBChangesEveryFrame
	{
		DirectX::XMMATRIX view;
		DirectX::XMVECTOR eyePos;
	};

	struct CBChangesOnResize
	{
		DirectX::XMMATRIX proj;
	};


	struct CBChangesRarely
	{
		DirectionalLight dirLight[BasicEffect::maxLights];
		PointLight pointLight[BasicEffect::maxLights];
		SpotLight spotLight[BasicEffect::maxLights];
		int numDirectLight;
		int numPointLight;
		int numSpotLight;
		float pad;
	};

public:
	// ������ʽָ��
	Impl() : m_IsDirty() {}
	~Impl() = default;

public:
	// ��Ҫ16�ֽڶ�������ȷ���ǰ��
	CBufferObject<0, CBChangesEveryDrawing> m_CBDrawing;        // ÿ�ζ�����Ƶĳ���������
	CBufferObject<1, CBChangesEveryFrame>   m_CBFrame;          // ÿ֡���Ƶĳ���������
	CBufferObject<2, CBChangesOnResize>     m_CBOnResize;       // ÿ�δ��ڴ�С����ĳ���������
	CBufferObject<3, CBChangesRarely>       m_CBRarely;         // �����������ĳ���������
	BOOL m_IsDirty;                                             // �Ƿ���ֵ���
	std::vector<CBufferBase*> m_pCBuffers;                      // ͳһ�����������еĳ���������


	ComPtr<ID3D11VertexShader> m_pVertexShader3D;               // ����3D�Ķ�����ɫ��
	ComPtr<ID3D11PixelShader>  m_pPixelShader3D;                // ����3D��������ɫ��
	ComPtr<ID3D11VertexShader> m_pVertexShader2D;               // ����2D�Ķ�����ɫ��
	ComPtr<ID3D11PixelShader>  m_pPixelShader2D;                // ����2D��������ɫ��

	ComPtr<ID3D11InputLayout>  m_pVertexLayout2D;               // ����2D�Ķ������벼��
	ComPtr<ID3D11InputLayout>  m_pVertexLayout3D;               // ����3D�Ķ������벼��

	ComPtr<ID3D11ShaderResourceView> m_pTexture;                // ���ڻ��Ƶ�����

};

namespace
{
	// BasicEffect����
	static BasicEffect* g_pInstance = new BasicEffect();
}

BasicEffect::BasicEffect()
{
	if (g_pInstance)
		throw std::exception("BasicEffect is a singleton!");
	g_pInstance = this;
	pImpl = std::make_unique<BasicEffect::Impl>();
}

BasicEffect::~BasicEffect()
{
}

BasicEffect::BasicEffect(BasicEffect&& moveFrom) noexcept
{
	pImpl.swap(moveFrom.pImpl);
}

BasicEffect& BasicEffect::operator=(BasicEffect&& moveFrom) noexcept
{
	pImpl.swap(moveFrom.pImpl);
	return *this;
}

BasicEffect& BasicEffect::Get()
{
	if (!g_pInstance)
		throw std::exception("BasicEffect needs an instance!");
	return *g_pInstance;
}

bool BasicEffect::InitAll(ID3D11Device* device)
{
	if (!device)
		return false;

	if (!pImpl->m_pCBuffers.empty())
		return true;

	if (!RenderStates::IsInit())
	{
		RenderStates::InitAll(device);
		//throw std::exception("RenderStates need to be initialized first!");
	}


	ComPtr<ID3DBlob> blob;

	// ����������ɫ��(3D)
	HR(CreateShaderFromFile(L"HLSL\\Basic_VS_3D.cso", L"HLSL\\LightVS.hlsl", "VS", "vs_5_0", blob.ReleaseAndGetAddressOf()));
	HR(device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, pImpl->m_pVertexShader3D.GetAddressOf()));
	// �������㲼��(3D)
	HR(device->CreateInputLayout(VertexPosNormalTex::inputLayout, ARRAYSIZE(VertexPosNormalTex::inputLayout),
		blob->GetBufferPointer(), blob->GetBufferSize(), pImpl->m_pVertexLayout3D.GetAddressOf()));

	// ����������ɫ��(3D)
	HR(CreateShaderFromFile(L"HLSL\\LightPS.cso", L"HLSL\\LightPS.hlsl", "PS", "ps_5_0", blob.ReleaseAndGetAddressOf()));
	HR(device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, pImpl->m_pPixelShader3D.GetAddressOf()));


	pImpl->m_pCBuffers.assign({
		&pImpl->m_CBDrawing,
		&pImpl->m_CBFrame,
		&pImpl->m_CBOnResize,
		&pImpl->m_CBRarely });

	// ��������������
	for (auto& pBuffer : pImpl->m_pCBuffers)
	{
		HR(pBuffer->CreateBuffer(device));
	}

	return true;
}
void BasicEffect::SetRenderDefault(ID3D11DeviceContext* deviceContext)
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
void BasicEffect::SetRenderAlphaBlend(ID3D11DeviceContext* deviceContext)
{
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	deviceContext->IASetInputLayout(pImpl->m_pVertexLayout3D.Get());
	deviceContext->VSSetShader(pImpl->m_pVertexShader3D.Get(), nullptr, 0);
	deviceContext->RSSetState(RenderStates::RSNoCull.Get());
	deviceContext->PSSetShader(pImpl->m_pPixelShader3D.Get(), nullptr, 0);
	deviceContext->PSSetSamplers(0, 1, RenderStates::SSLinearWrap.GetAddressOf());
	deviceContext->OMSetDepthStencilState(nullptr, 0);
	deviceContext->OMSetBlendState(RenderStates::BSTransparent.Get(), nullptr, 0xFFFFFFFF);
}
void BasicEffect::SetRenderNoDoubleBlend(ID3D11DeviceContext* deviceContext, UINT stencilRef)
{
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	deviceContext->IASetInputLayout(pImpl->m_pVertexLayout3D.Get());
	deviceContext->VSSetShader(pImpl->m_pVertexShader3D.Get(), nullptr, 0);
	deviceContext->RSSetState(RenderStates::RSNoCull.Get());
	deviceContext->PSSetShader(pImpl->m_pPixelShader3D.Get(), nullptr, 0);
	deviceContext->PSSetSamplers(0, 1, RenderStates::SSLinearWrap.GetAddressOf());
	deviceContext->OMSetDepthStencilState(RenderStates::DSSNoDoubleBlend.Get(), stencilRef);
	deviceContext->OMSetBlendState(RenderStates::BSTransparent.Get(), nullptr, 0xFFFFFFFF);
}
void XM_CALLCONV BasicEffect::SetWorldMatrix(DirectX::FXMMATRIX W)
{
	auto& cBuffer = pImpl->m_CBDrawing;
	cBuffer.data.world = XMMatrixTranspose(W);
	cBuffer.data.worldInvTranspose = XMMatrixInverse(nullptr, W);   // ����ת�õ���
	pImpl->m_IsDirty = cBuffer.isDirty = true;
}

void XM_CALLCONV BasicEffect::SetViewMatrix(FXMMATRIX V)
{
	auto& cBuffer = pImpl->m_CBFrame;
	cBuffer.data.view = XMMatrixTranspose(V);
	pImpl->m_IsDirty = cBuffer.isDirty = true;
}

void XM_CALLCONV BasicEffect::SetProjMatrix(FXMMATRIX P)
{
	auto& cBuffer = pImpl->m_CBOnResize;
	cBuffer.data.proj = XMMatrixTranspose(P);
	pImpl->m_IsDirty = cBuffer.isDirty = true;
}


void BasicEffect::SetDirLight(size_t pos, const DirectionalLight& dirLight)
{
	auto& cBuffer = pImpl->m_CBRarely;
	cBuffer.data.dirLight[pos] = dirLight;
	cBuffer.data.numDirectLight = max(cBuffer.data.numDirectLight, static_cast<int>(pos));
	pImpl->m_IsDirty = cBuffer.isDirty = true;
}

void BasicEffect::SetPointLight(size_t pos, const PointLight& pointLight)
{
	auto& cBuffer = pImpl->m_CBRarely;
	cBuffer.data.pointLight[pos] = pointLight;
	cBuffer.data.numPointLight = max(cBuffer.data.numPointLight, static_cast<int>(pos));
	pImpl->m_IsDirty = cBuffer.isDirty = true;
}

void BasicEffect::SetSpotLight(size_t pos, const SpotLight& spotLight)
{
	auto& cBuffer = pImpl->m_CBRarely;
	cBuffer.data.spotLight[pos] = spotLight;
	cBuffer.data.numSpotLight = max(cBuffer.data.numSpotLight, static_cast<int>(pos));
	pImpl->m_IsDirty = cBuffer.isDirty = true;
}


void BasicEffect::SetMaterial(const Material& material)
{
	auto& cBuffer = pImpl->m_CBDrawing;
	cBuffer.data.material = material;
	pImpl->m_IsDirty = cBuffer.isDirty = true;
}

void BasicEffect::SetTexture(ID3D11ShaderResourceView* m_pTexture)
{
	pImpl->m_pTexture = m_pTexture;
}

void XM_CALLCONV BasicEffect::SetEyePos(FXMVECTOR eyePos)
{
	auto& cBuffer = pImpl->m_CBFrame;
	cBuffer.data.eyePos = eyePos;
	pImpl->m_IsDirty = cBuffer.isDirty = true;
}


void BasicEffect::Apply(ID3D11DeviceContext* deviceContext)
{
	auto& pCBuffers = pImpl->m_pCBuffers;
	// ���������󶨵���Ⱦ������
	pCBuffers[0]->BindVS(deviceContext);
	pCBuffers[1]->BindVS(deviceContext);
	pCBuffers[2]->BindVS(deviceContext);

	pCBuffers[0]->BindPS(deviceContext);
	pCBuffers[1]->BindPS(deviceContext);
	pCBuffers[3]->BindPS(deviceContext);

	// ��������
	deviceContext->PSSetShaderResources(0, 1, pImpl->m_pTexture.GetAddressOf());

	if (pImpl->m_IsDirty)
	{
		pImpl->m_IsDirty = false;
		for (auto& pCBuffer : pCBuffers)
		{
			pCBuffer->UpdateBuffer(deviceContext);
		}
	}
}