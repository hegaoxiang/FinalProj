#include "GameApp.h"
#include "d3dUtil.h"
#include "DXTrace.h"

using namespace DirectX;

struct ConstantBuffer
{
	XMMATRIX world;
	XMMATRIX view;
	XMMATRIX proj;
};
GameApp::GameApp(HINSTANCE hInstance)
	: D3DApp(hInstance)
{
}

GameApp::~GameApp()
{
	
}

bool GameApp::Init()
{
	if (!D3DApp::Init())
		return false;
	if (!InitShader())
		return false;
	if (!InitResource())
		return false;
	return true;
}

void GameApp::OnResize()
{
	D3DApp::OnResize();
}

void GameApp::UpdateScene(float dt)
{
	
	// Start the Dear ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();


	static int i = 0;
	static XMFLOAT4 f1(0.0f, 0.0f, -5.0f, 0.0f);
	static XMFLOAT4 f2(0.0f, 0.0f, 0.0f, 0.0f);
	static XMFLOAT4 f3(0.0f, 1.0f, 0.0f, 0.0f);
	ImGui::Begin("Editor");   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
	ImGui::ColorEdit4("colorbg", (float*)& m_bg);
	//ImGui::SliderFloat4("")
	ImGui::SliderInt("textureIdx", &i, 0, 5);
	ImGui::SliderFloat4("1", (float*)& f1,-10.f, 10.f);
	ImGui::SliderFloat4("2", (float*)& f2, -10.f, 10.f);
	ImGui::SliderFloat4("3", (float*)& f3, -10.f, 10.f);
	ImGui::End();
	ConstantBuffer m_CBuffer;
	m_CBuffer.world = XMMatrixMultiplyTranspose(XMMatrixIdentity(), XMMatrixTranslation(2.f, 2.f, 1.f));   // 单位矩阵的转置是它本身
	m_CBuffer.view = XMMatrixTranspose(XMMatrixLookAtLH(		
		XMLoadFloat4(&f1),
		XMLoadFloat4(&f2),
		XMLoadFloat4(&f3)
	));

	m_CBuffer.proj = XMMatrixTranspose(XMMatrixPerspectiveFovLH(XM_PIDIV2, AspectRatio(), 1.0f, 1000.0f));
	static float phi = 0.0f, theta = 0.0f;
	phi += 0.0001f, theta += 0.00015f;
	m_CBuffer.world *= (XMMatrixRotationX(phi) * XMMatrixRotationY(theta));
	
	
	
	m_pd3dImmediateContext->PSSetShaderResources(0, 1, m_pTemp[i].GetAddressOf());

	D3D11_MAPPED_SUBRESOURCE mappedData;
	HR(m_pd3dImmediateContext->Map(m_pConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));
	memcpy_s(mappedData.pData, sizeof(m_CBuffer), &m_CBuffer, sizeof(m_CBuffer));
	m_pd3dImmediateContext->Unmap(m_pConstantBuffer.Get(), 0);
}

void GameApp::DrawScene()
{

	assert(m_pd3dImmediateContext);
	assert(m_pSwapChain);

	ImGui::Render();
	
	m_pd3dImmediateContext->ClearRenderTargetView(m_pRenderTargetView.Get(), m_bg);
	m_pd3dImmediateContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	for (int i = 0; i < 6; i++)
	{
		m_pd3dImmediateContext->PSSetShaderResources(0, 1, m_pTemp[i].GetAddressOf());
		m_pd3dImmediateContext->DrawIndexed(m_IndexCount / 6,i* m_IndexCount / 6, 0);
	}
	
	
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	HR(m_pSwapChain->Present(0, 0));
}

bool GameApp::InitShader()
{
	ComPtr<ID3DBlob> blob;

	// 创建顶点着色器
	HR(CreateShaderFromFile(L"HLSL\\Triangle_VS02.cso", L"HLSL\\Triangle_VS02.hlsl", "VS", "vs_5_0", blob.ReleaseAndGetAddressOf()));
	HR(m_pd3dDevice->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_pVertexShader.GetAddressOf()));
	// 创建并绑定顶点布局
	HR(m_pd3dDevice->CreateInputLayout(VertexPosTex::inputLayout, ARRAYSIZE(VertexPosTex::inputLayout),
		blob->GetBufferPointer(), blob->GetBufferSize(), m_pVertexLayout.GetAddressOf()));

	// 创建像素着色器
	HR(CreateShaderFromFile(L"HLSL\\Triangle_PS01.cso", L"HLSL\\Triangle_PS01.hlsl", "PS", "ps_5_0", blob.ReleaseAndGetAddressOf()));
	HR(m_pd3dDevice->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_pPixelShader.GetAddressOf()));

	return true;
}

bool GameApp::InitResource()
{
	// 初始化木箱纹理
	auto mesh = Geometry::CreateBox<VertexPosTex,WORD>();
	ResetMesh<VertexPosTex>(mesh);

	D3D11_BUFFER_DESC cbd;
	ZeroMemory(&cbd, sizeof(cbd));
	cbd.Usage = D3D11_USAGE_DYNAMIC;	
	cbd.ByteWidth = sizeof(ConstantBuffer);
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	// 新建常量缓冲区，不使用初始数据
	HR(m_pd3dDevice->CreateBuffer(&cbd, nullptr, m_pConstantBuffer.GetAddressOf()));

	m_pd3dImmediateContext->VSSetConstantBuffers(0, 1, m_pConstantBuffer.GetAddressOf());
	// 设置图元类型，设定输入布局
	m_pd3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_pd3dImmediateContext->IASetInputLayout(m_pVertexLayout.Get());
	
	// 初始化采样器状态描述
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	HR(m_pd3dDevice->CreateSamplerState(&sampDesc, m_pSamplerState.GetAddressOf()));

	HR(CreateDDSTextureFromFile(m_pd3dDevice.Get(), L"Texture\\WoodCrate.dds", nullptr, m_pWoodCrate.GetAddressOf()));
	HR(CreateDDSTextureFromFile(m_pd3dDevice.Get(), L"Texture\\Blue.dds", nullptr, m_pTemp[0].GetAddressOf()));
	HR(CreateDDSTextureFromFile(m_pd3dDevice.Get(), L"Texture\\Green.dds", nullptr, m_pTemp[1].GetAddressOf()));
	HR(CreateDDSTextureFromFile(m_pd3dDevice.Get(), L"Texture\\Orange.dds", nullptr, m_pTemp[2].GetAddressOf()));
	HR(CreateDDSTextureFromFile(m_pd3dDevice.Get(), L"Texture\\Red.dds", nullptr, m_pTemp[3].GetAddressOf()));
	HR(CreateDDSTextureFromFile(m_pd3dDevice.Get(), L"Texture\\White.dds", nullptr, m_pTemp[4].GetAddressOf()));
	HR(CreateDDSTextureFromFile(m_pd3dDevice.Get(), L"Texture\\yellow.dds", nullptr, m_pTemp[5].GetAddressOf()));
	
	
	m_pd3dImmediateContext->PSSetShaderResources(0, 1, m_pWoodCrate.GetAddressOf());
	// 像素着色阶段设置好采样器
	m_pd3dImmediateContext->PSSetSamplers(0, 1, m_pSamplerState.GetAddressOf());

	// 将着色器绑定到渲染管线
	m_pd3dImmediateContext->VSSetShader(m_pVertexShader.Get(), nullptr, 0);
	m_pd3dImmediateContext->PSSetShader(m_pPixelShader.Get(), nullptr, 0);
	return true;
}
template<class VertexType>
bool GameApp::ResetMesh(const Geometry::MeshData<VertexType>& meshData)
{

	// 释放旧资源
	m_pVertexBuffer.Reset();
	m_pIndexBuffer.Reset();

	// 设置顶点缓冲区描述
	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = (UINT)meshData.vertexVec.size() * sizeof(VertexType);
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;

	// 新建顶点缓冲区
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = meshData.vertexVec.data();
	HR(m_pd3dDevice->CreateBuffer(&vbd, &InitData, m_pVertexBuffer.GetAddressOf()));

	// 输入装配阶段的顶点缓冲区设置
	UINT stride = sizeof(VertexType);	// 跨越字节数
	UINT offset = 0;							// 起始偏移量

	m_pd3dImmediateContext->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), &stride, &offset);

	// 设置索引缓冲区描述
	m_IndexCount = (UINT)meshData.indexVec.size();
	D3D11_BUFFER_DESC ibd;
	ZeroMemory(&ibd, sizeof(ibd));
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = m_IndexCount * sizeof(WORD);
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	// 新建索引缓冲区
	InitData.pSysMem = meshData.indexVec.data();
	HR(m_pd3dDevice->CreateBuffer(&ibd, &InitData, m_pIndexBuffer.GetAddressOf()));
	// 输入装配阶段的索引缓冲区设置
	m_pd3dImmediateContext->IASetIndexBuffer(m_pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
	// 设置调试对象名
	D3D11SetDebugObjectName(m_pVertexBuffer.Get(), "VertexBuffer");
	D3D11SetDebugObjectName(m_pIndexBuffer.Get(), "IndexBuffer");

	return true;
}
