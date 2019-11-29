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
	m_pMouse->SetMode(Mouse::Mode::MODE_RELATIVE);
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
	// Start the Dear ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// 更新鼠标事件，获取相对偏移量
	auto mouseState = m_pMouse->GetState();
	auto lastMouseState = m_MouseTracker.GetLastState();

	auto keyState = m_pKeyboard->GetState();
	m_KeyboardTracker.Update(keyState);

	CBChangesEveryFrame cbEveryFrame;
	

	// 获取子类
	auto cam1st = std::dynamic_pointer_cast<FirstPersonCamera>(m_pCamera);
	auto cam3rd = std::dynamic_pointer_cast<ThirdPersonCamera>(m_pCamera);

	if (m_CameraMode == CameraMode::FirstPerson || m_CameraMode == CameraMode::Free)
	{
		// 第一人称/自由摄像机的操作

		// 方向移动
		if (keyState.IsKeyDown(Keyboard::W))
		{
			if (m_CameraMode == CameraMode::FirstPerson)
				cam1st->Walk(dt * 3.0f);
			else
				cam1st->MoveForward(dt * 3.0f);
		}
		if (keyState.IsKeyDown(Keyboard::S))
		{
			if (m_CameraMode == CameraMode::FirstPerson)
				cam1st->Walk(dt * -3.0f);
			else
				cam1st->MoveForward(dt * -3.0f);
		}
		if (keyState.IsKeyDown(Keyboard::A))
			cam1st->Strafe(dt * -3.0f);
		if (keyState.IsKeyDown(Keyboard::D))
			cam1st->Strafe(dt * 3.0f);

		// 将位置限制在[-8.9f, 8.9f]的区域内
		// 不允许穿地
		XMFLOAT3 adjustedPos;
		XMStoreFloat3(&adjustedPos, XMVectorClamp(cam1st->GetPositionXM(), XMVectorSet(-8.9f, 0.0f, -8.9f, 0.0f), XMVectorReplicate(8.9f)));
		cam1st->SetPosition(adjustedPos);

		// 仅在第一人称模式移动箱子
		if (m_CameraMode == CameraMode::FirstPerson)
			m_WoodCrate.SetWorldMatrix(XMMatrixTranslation(adjustedPos.x, adjustedPos.y, adjustedPos.z));
		// 视野旋转，防止开始的差值过大导致的突然旋转
		cam1st->Pitch(mouseState.y * dt * 1.25f);
		cam1st->RotateY(mouseState.x * dt * 1.25f);
	}
	else if (m_CameraMode == CameraMode::ThirdPerson)
	{
		// 第三人称摄像机的操作

		cam3rd->SetTarget(m_WoodCrate.GetPosition());

		// 绕物体旋转
		cam3rd->RotateX(mouseState.y * dt * 1.25f);
		cam3rd->RotateY(mouseState.x * dt * 1.25f);
		cam3rd->Approach(-mouseState.scrollWheelValue / 120 * 1.0f);
	}

	// 更新观察矩阵
	m_pCamera->UpdateViewMatrix();
	//XMStoreFloat4(&cbEveryFrame.eyePos, m_pCamera->GetPositionXM());
	cbEveryFrame.view = XMMatrixTranspose(m_pCamera->GetViewXM());

	// 重置滚轮值
	m_pMouse->ResetScrollWheelValue();

	// 摄像机模式切换
	if (m_KeyboardTracker.IsKeyPressed(Keyboard::D1) && m_CameraMode != CameraMode::FirstPerson)
	{
		if (!cam1st)
		{
			cam1st.reset(new FirstPersonCamera);
			cam1st->SetFrustum(XM_PI / 3, AspectRatio(), 0.5f, 1000.0f);
			m_pCamera = cam1st;
		}

		cam1st->LookTo(m_WoodCrate.GetPosition(),
			XMFLOAT3(0.0f, 0.0f, 1.0f),
			XMFLOAT3(0.0f, 1.0f, 0.0f));

		m_CameraMode = CameraMode::FirstPerson;
	}
	else if (m_KeyboardTracker.IsKeyPressed(Keyboard::D2) && m_CameraMode != CameraMode::ThirdPerson)
	{
		if (!cam3rd)
		{
			cam3rd.reset(new ThirdPersonCamera);
			cam3rd->SetFrustum(XM_PI / 3, AspectRatio(), 0.5f, 1000.0f);
			m_pCamera = cam3rd;
		}
		XMFLOAT3 target = m_WoodCrate.GetPosition();
		cam3rd->SetTarget(target);
		cam3rd->SetDistance(8.0f);
		cam3rd->SetDistanceMinMax(3.0f, 20.0f);

		m_CameraMode = CameraMode::ThirdPerson;
	}
	else if (m_KeyboardTracker.IsKeyPressed(Keyboard::D3) && m_CameraMode != CameraMode::Free)
	{
		if (!cam1st)
		{
			cam1st.reset(new FirstPersonCamera);
			cam1st->SetFrustum(XM_PI / 3, AspectRatio(), 0.5f, 1000.0f);
			m_pCamera = cam1st;
		}
		// 从箱子上方开始
		XMFLOAT3 pos = m_WoodCrate.GetPosition();
		XMFLOAT3 to = XMFLOAT3(0.0f, 0.0f, 1.0f);
		XMFLOAT3 up = XMFLOAT3(0.0f, 1.0f, 0.0f);
		pos.y += 3;
		cam1st->LookTo(pos, to, up);

		m_CameraMode = CameraMode::Free;
	}
	// 退出程序，这里应向窗口发送销毁信息
	if (keyState.IsKeyDown(Keyboard::Escape))
		SendMessage(MainWnd(), WM_DESTROY, 0, 0);
	
	ImGui::Begin("Editor");   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
	ImGui::ColorEdit4("colorbg", (float*)& m_bg);

	//ImGui::SliderInt("textureIdx", &i, 0, 5);
	
	ImGui::End();
	

	
	
	
	D3D11_MAPPED_SUBRESOURCE mappedData;
	
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
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	for (size_t i = 0; i < 2; i++)
	{
		
		if (i == 1)
		{
			m_ScreenViewport.TopLeftX = 0;
			m_pd3dImmediateContext->RSSetViewports(1, &m_ScreenViewport);
		}
		else
		{
			m_ScreenViewport.TopLeftX = static_cast<float>(m_ClientWidth) / 2;
			m_pd3dImmediateContext->RSSetViewports(1, &m_ScreenViewport);
		}
		


		m_pd3dImmediateContext->PSSetShaderResources(0, 1, m_WoodCrate.m_pTexture.GetAddressOf());
		m_WoodCrate.Draw(m_pd3dImmediateContext.Get());
	
		m_pd3dImmediateContext->PSSetShaderResources(0, 1, m_Floor.m_pTexture.GetAddressOf());
		m_Floor.Draw(m_pd3dImmediateContext.Get());

		for (auto& wall : m_Walls)
		{
			m_pd3dImmediateContext->PSSetShaderResources(0, 1, wall.m_pTexture.GetAddressOf());
			wall.Draw(m_pd3dImmediateContext.Get());
		}
		
	}


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
	m_CameraMode = CameraMode::FirstPerson;

	auto camera = std::shared_ptr<FirstPersonCamera>(new FirstPersonCamera);

	m_pCamera = camera;

	camera->SetViewPort(0.0f, 0.0f, (float)m_ClientWidth, (float)m_ClientHeight);

	camera->LookAt(XMFLOAT3(), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
	// 初始化仅在窗口大小变动时修改的值

	m_pCamera->SetFrustum(XM_PI / 3, AspectRatio(), 0.5f, 1000.0f);

	m_CBOnResize.proj = XMMatrixTranspose(m_pCamera->GetProjXM());


	// 初始化游戏对象
	ComPtr<ID3D11ShaderResourceView> texture;

	HR(CreateDDSTextureFromFile(m_pd3dDevice.Get(), L"Texture\\WoodCrate.dds", nullptr, texture.GetAddressOf()));
	m_WoodCrate.SetBuffer(m_pd3dDevice.Get(), Geometry::CreateBox<VertexPosTex>());
	m_WoodCrate.SetTexture(texture.Get());

	// 初始化地板
	HR(CreateDDSTextureFromFile(m_pd3dDevice.Get(), L"Texture\\Green.dds", nullptr, texture.ReleaseAndGetAddressOf()));
	m_Floor.SetBuffer(m_pd3dDevice.Get(),
		Geometry::CreatePlane<VertexPosTex>(XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(20.0f, 20.0f), XMFLOAT2(5.0f, 5.0f)));
	m_Floor.SetTexture(texture.Get());

	// 初始化墙体
	m_Walls.resize(4);
	HR(CreateDDSTextureFromFile(m_pd3dDevice.Get(), L"Texture\\Orange.dds", nullptr, texture.ReleaseAndGetAddressOf()));
	// 这里控制墙体四个面的生成
	for (int i = 0; i < 4; ++i)
	{
		m_Walls[i].SetBuffer(m_pd3dDevice.Get(),
			Geometry::CreatePlane<VertexPosTex>(XMFLOAT3(), XMFLOAT2(20.0f, 8.0f), XMFLOAT2(5.0f, 1.5f)));
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
	
	// 像素着色阶段设置好采样器
	m_pd3dImmediateContext->PSSetSamplers(0, 1, m_pSamplerState.GetAddressOf());

	// 更新不容易被修改的常量缓冲区资源

	D3D11_MAPPED_SUBRESOURCE mappedData;
	HR(m_pd3dImmediateContext->Map(m_pConstantBuffers[2].Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));
	memcpy_s(mappedData.pData, sizeof(CBChangesOnResize), &m_CBOnResize, sizeof(CBChangesOnResize));
	m_pd3dImmediateContext->Unmap(m_pConstantBuffers[2].Get(), 0);

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
