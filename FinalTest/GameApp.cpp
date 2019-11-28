#include "GameApp.h"
#include "d3dUtil.h"
#include "DXTrace.h"

using namespace DirectX;


GameApp::GameApp(HINSTANCE hInstance)
	: D3DApp(hInstance)
{
	
}

GameApp::~GameApp()
{
	
}

bool GameApp::Init()
{
	m_pMouse = std::make_unique<Mouse>();
	m_pKeyboard = std::make_unique<Keyboard>();

	

	if (!D3DApp::Init())
		return false;
	if (!InitShader())
		return false;
	if (!InitResource())
		return false;

	m_pMouse->SetWindow(m_hMainWnd);
	m_pMouse->SetMode(Mouse::Mode::MODE_ABSOLUTE);
	return true;
}

void GameApp::OnResize()
{
	D3DApp::OnResize();
	if (m_pCamera != nullptr)
	{
		m_pCamera->SetFrustum(XM_PI / 3, AspectRatio(), 0.5f, 1000.f);
		m_pCamera->SetViewPort(m_ScreenViewport);


		CBChangesOnResize cbOnResize;
		cbOnResize.proj = XMMatrixTranspose(m_pCamera->GetProjXM());
		
		D3D11_MAPPED_SUBRESOURCE mappedData;
		HR(m_pd3dImmediateContext->Map(m_pConstantBuffers[2].Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));
		memcpy_s(mappedData.pData, sizeof(CBChangesOnResize), &cbOnResize, sizeof(CBChangesOnResize));
		m_pd3dImmediateContext->Unmap(m_pConstantBuffers[2].Get(), 0);

	}
}

void GameApp::UpdateScene(float dt)
{
	m_pCamera->UpdateViewMatrix();
	// Start the Dear ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();


	static int i = 0;
	
	ImGui::Begin("Editor");   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
	ImGui::ColorEdit4("colorbg", (float*)& m_bg);

	ImGui::SliderInt("textureIdx", &i, 0, 5);
	
	ImGui::End();
	

	CBChangesEveryDrawing cbEveryDraw;
	CBChangesEveryFrame cbEveryFrame;
	cbEveryDraw.world = XMMatrixMultiplyTranspose(XMMatrixIdentity(), XMMatrixTranslation(2.f, 2.f, 1.f));   // 单位矩阵的转置是它本身
	cbEveryFrame.view = XMMatrixTranspose(m_pCamera->GetViewXM());
	

	static float phi = 0.0f, theta = 0.0f;
	phi += 0.0001f, theta += 0.00015f;
	cbEveryDraw.world *= (XMMatrixRotationX(phi) * XMMatrixRotationY(theta));
	
	
	
	m_pd3dImmediateContext->PSSetShaderResources(0, 1, m_pTemp[i].GetAddressOf());

	D3D11_MAPPED_SUBRESOURCE mappedData;
	HR(m_pd3dImmediateContext->Map(m_pConstantBuffers[0].Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));
	memcpy_s(mappedData.pData, sizeof(CBChangesEveryDrawing), &cbEveryDraw, sizeof(CBChangesEveryDrawing));
	m_pd3dImmediateContext->Unmap(m_pConstantBuffers[0].Get(), 0);
	
	HR(m_pd3dImmediateContext->Map(m_pConstantBuffers[1].Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));
	memcpy_s(mappedData.pData, sizeof(CBChangesEveryFrame), &cbEveryFrame, sizeof(CBChangesEveryFrame));
	m_pd3dImmediateContext->Unmap(m_pConstantBuffers[1].Get(), 0);
}

void GameApp::DrawScene()
{

	assert(m_pd3dImmediateContext);
	assert(m_pSwapChain);

	ImGui::Render();
	
	m_pd3dImmediateContext->ClearRenderTargetView(m_pRenderTargetView.Get(), m_bg);
	m_pd3dImmediateContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	m_pd3dImmediateContext->DrawIndexed(m_IndexCount, 0, 0);
	/*
	for (int i = 0; i < 6; i++)
	{
		m_pd3dImmediateContext->PSSetShaderResources(0, 1, m_pTemp[i].GetAddressOf());
		m_pd3dImmediateContext->DrawIndexed(m_IndexCount / 6, i * m_IndexCount / 6, 0);
	}*/
	
	
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	HR(m_pSwapChain->Present(0, 0));
}

bool GameApp::InitShader()
{
	ComPtr<ID3DBlob> blob;

	// 创建顶点着色器
	HR(CreateShaderFromFile(L"HLSL\\Triangle_VS03.cso", L"HLSL\\Triangle_VS03.hlsl", "VS", "vs_5_0", blob.ReleaseAndGetAddressOf()));
	HR(m_pd3dDevice->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_pVertexShader.GetAddressOf()));
	// 创建并绑定顶点布局
	HR(m_pd3dDevice->CreateInputLayout(VertexPosTex::inputLayout, ARRAYSIZE(VertexPosTex::inputLayout),
		blob->GetBufferPointer(), blob->GetBufferSize(), m_pVertexLayout.GetAddressOf()));

	// 创建像素着色器
	HR(CreateShaderFromFile(L"HLSL\\Triangle_PS02.cso", L"HLSL\\Triangle_PS02.hlsl", "PS", "ps_5_0", blob.ReleaseAndGetAddressOf()));
	HR(m_pd3dDevice->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_pPixelShader.GetAddressOf()));

	return true;
}

bool GameApp::InitResource()
{
	m_pCamera = std::make_unique<FirstPersonCamera>();
	// 初始化相机
	XMFLOAT4 f1(0.0f, 0.0f, -5.0f, 0.0f);
	XMFLOAT4 f2(0.0f, 0.0f, 0.0f, 0.0f);
	XMFLOAT4 f3(0.0f, 1.0f, 0.0f, 0.0f);
	m_pCamera->SetPosition(0, 0, -5.f);
	m_pCamera->LookAt(XMLoadFloat4(&f1),
		XMLoadFloat4(&f2),
		XMLoadFloat4(&f3));
	m_pCamera->SetFrustum(XM_PI / 3, AspectRatio(), 0.5f, 1000.f);
	
	m_pCamera->SetViewPort(m_ScreenViewport);

	// 初始化游戏对象
	ComPtr<ID3D11ShaderResourceView> texture;

	HR(CreateDDSTextureFromFile(m_pd3dDevice.Get(), L"Texture\\WoodCrate.dds", nullptr, texture.GetAddressOf()));
	m_WoodCrate.SetBuffer(m_pd3dDevice.Get(), Geometry::CreateBox());
	m_WoodCrate.SetTexture(texture.Get());

	// 初始化地板
	HR(CreateDDSTextureFromFile(m_pd3dDevice.Get(), L"Texture\\Green.dds", nullptr, texture.ReleaseAndGetAddressOf()));
	m_Floor.SetBuffer(m_pd3dDevice.Get(),
		Geometry::CreatePlane(XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(20.0f, 20.0f), XMFLOAT2(5.0f, 5.0f)));
	m_Floor.SetTexture(texture.Get());

	// 初始化墙体
	m_Walls.resize(4);
	HR(CreateDDSTextureFromFile(m_pd3dDevice.Get(), L"Texture\\Orange.dds", nullptr, texture.ReleaseAndGetAddressOf()));
	// 这里控制墙体四个面的生成
	for (int i = 0; i < 4; ++i)
	{
		m_Walls[i].SetBuffer(m_pd3dDevice.Get(),
			Geometry::CreatePlane(XMFLOAT3(), XMFLOAT2(20.0f, 8.0f), XMFLOAT2(5.0f, 1.5f)));
		XMMATRIX world = XMMatrixRotationX(-XM_PIDIV2) * XMMatrixRotationY(XM_PIDIV2 * i)
			* XMMatrixTranslation(i % 2 ? -10.0f * (i - 2) : 0.0f, 3.0f, i % 2 == 0 ? -10.0f * (i - 1) : 0.0f);
		m_Walls[i].SetWorldMatrix(world);
		m_Walls[i].SetTexture(texture.Get());
	}

	// 设置常量缓冲区描述
	D3D11_BUFFER_DESC cbd;
	ZeroMemory(&cbd, sizeof(cbd));
	cbd.Usage = D3D11_USAGE_DYNAMIC;
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	// 新建用于VS和PS的常量缓冲区
	cbd.ByteWidth = sizeof(CBChangesEveryDrawing);
	HR(m_pd3dDevice->CreateBuffer(&cbd, nullptr, m_pConstantBuffers[0].GetAddressOf()));
	cbd.ByteWidth = sizeof(CBChangesEveryFrame);
	HR(m_pd3dDevice->CreateBuffer(&cbd, nullptr, m_pConstantBuffers[1].GetAddressOf()));
	cbd.ByteWidth = sizeof(CBChangesOnResize);
	HR(m_pd3dDevice->CreateBuffer(&cbd, nullptr, m_pConstantBuffers[2].GetAddressOf()));

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

	// 预先绑定各自所需的缓冲区，其中每帧更新的缓冲区需要绑定到两个缓冲区上
	m_pd3dImmediateContext->VSSetConstantBuffers(0, 1, m_pConstantBuffers[0].GetAddressOf());
	m_pd3dImmediateContext->VSSetConstantBuffers(1, 1, m_pConstantBuffers[1].GetAddressOf());
	m_pd3dImmediateContext->VSSetConstantBuffers(2, 1, m_pConstantBuffers[2].GetAddressOf());

	m_pd3dImmediateContext->PSSetConstantBuffers(1, 1, m_pConstantBuffers[1].GetAddressOf());
	

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
	UINT offset = 0;					// 起始偏移量

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
