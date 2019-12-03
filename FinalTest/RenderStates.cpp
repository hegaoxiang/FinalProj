#include "RenderStates.h"
#include "DXTrace.h"
using namespace Microsoft::WRL;

ComPtr<ID3D11RasterizerState> RenderStates::RSNoCull = nullptr;
ComPtr<ID3D11RasterizerState> RenderStates::RSWireframe = nullptr;

ComPtr<ID3D11SamplerState> RenderStates::SSAnistropicWrap = nullptr;
ComPtr<ID3D11SamplerState> RenderStates::SSLinearWrap = nullptr;

ComPtr<ID3D11BlendState> RenderStates::BSAlphaToCoverage = nullptr;
ComPtr<ID3D11BlendState> RenderStates::BSNoColorWrite = nullptr;
ComPtr<ID3D11BlendState> RenderStates::BSTransparent = nullptr;

void RenderStates::InitAll(ID3D11Device* device)
{
	// ��ǰ��ʼ�����Ļ���û��Ҫ������
	if (IsInit())
		return;

	// ***********��ʼ����դ����״̬***********
	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(rasterizerDesc));

	// �߿�ģʽ
	rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	rasterizerDesc.FrontCounterClockwise = false;
	rasterizerDesc.DepthClipEnable = true;
	HR(device->CreateRasterizerState(&rasterizerDesc, RSWireframe.GetAddressOf()));

	// �ޱ����޳�ģʽ
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	rasterizerDesc.FrontCounterClockwise = false;
	rasterizerDesc.DepthClipEnable = true;
	HR(device->CreateRasterizerState(&rasterizerDesc, RSNoCull.GetAddressOf()));


	// ***********��ʼ��������״̬***********
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));

	// ���Թ���ģʽ
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	HR(device->CreateSamplerState(&sampDesc, SSLinearWrap.GetAddressOf()));

	// �������Թ���ģʽ
	sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MaxAnisotropy = 4;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	HR(device->CreateSamplerState(&sampDesc, SSAnistropicWrap.GetAddressOf()));

	// ***********��ʼ�����״̬***********
	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(blendDesc));
	auto& rtDesc = blendDesc.RenderTarget[0];
	// Alpha-To-Coverageģʽ
	blendDesc.AlphaToCoverageEnable = true;
	blendDesc.IndependentBlendEnable = false;
	rtDesc.BlendEnable = false;
	rtDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	HR(device->CreateBlendState(&blendDesc, BSAlphaToCoverage.GetAddressOf()));

	// ͸�����ģʽ
	// Color = SrcAlpha * SrcColor + (1 - SrcAlpha) * DestColor 
	// Alpha = SrcAlpha
	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.IndependentBlendEnable = false;
	rtDesc.BlendEnable = true;
	rtDesc.SrcBlend = D3D11_BLEND_SRC_ALPHA;
	rtDesc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	rtDesc.BlendOp = D3D11_BLEND_OP_ADD;
	rtDesc.SrcBlendAlpha = D3D11_BLEND_ONE;
	rtDesc.DestBlendAlpha = D3D11_BLEND_ZERO;
	rtDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;

	HR(device->CreateBlendState(&blendDesc, BSTransparent.GetAddressOf()));

	// ����ɫд����ģʽ
	// Color = DestColor
	// Alpha = DestAlpha
	rtDesc.SrcBlend = D3D11_BLEND_ZERO;
	rtDesc.DestBlend = D3D11_BLEND_ONE;
	rtDesc.BlendOp = D3D11_BLEND_OP_ADD;
	rtDesc.SrcBlendAlpha = D3D11_BLEND_ZERO;
	rtDesc.DestBlendAlpha = D3D11_BLEND_ONE;
	rtDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	HR(device->CreateBlendState(&blendDesc, BSNoColorWrite.GetAddressOf()));

}

bool RenderStates::IsInit()
{
	if (RSWireframe == nullptr
		|| RSNoCull == nullptr
		|| SSLinearWrap == nullptr
		|| SSAnistropicWrap == nullptr
		|| BSNoColorWrite == nullptr
		|| BSTransparent == nullptr
		|| BSAlphaToCoverage == nullptr)
		return false;
	return true;
	
}
