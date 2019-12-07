#include "GameApp.h"
#include "d3dUtil.h"
#include "DXTrace.h"
#include "BasicEffect.h"
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


		BasicEffect::Get().SetProjMatrix(m_pCamera->GetProjXM());
	}
}

void GameApp::UpdateScene(float dt)
{
	// Start the Dear ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// ��������¼�����ȡ���ƫ����
	auto mouseState = m_pMouse->GetState();
	auto lastMouseState = m_MouseTracker.GetLastState();
	m_MouseTracker.Update(mouseState);

	mouseState.x = mouseState.x - lastMouseState.x;
	mouseState.y = mouseState.y - lastMouseState.y;
	


	auto keyState = m_pKeyboard->GetState();
	m_KeyboardTracker.Update(keyState);
	
	int w = mouseState.rightButton;

	
	ImGui::Begin("Editor");   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
	ImGui::ColorEdit4("colorbg", (float*)&m_bg);

	ImGui::DragInt("button", &w);
	//ImGui::SliderInt("textureIdx", &i, 0, 5);

	ImGui::End();

	static Mouse::State noOperation = {};
	if (m_MouseTracker.rightButton == Mouse::ButtonStateTracker::HELD)
	{
		m_pMouse->SetVisible(false);
	}else
	{
		m_pMouse->SetVisible(true);	
		mouseState = noOperation;
	}
	
	// ��ȡ����
	auto cam1st = std::dynamic_pointer_cast<FirstPersonCamera>(m_pCamera);
	auto cam3rd = std::dynamic_pointer_cast<ThirdPersonCamera>(m_pCamera);

	if (m_CameraMode == CameraMode::FirstPerson || m_CameraMode == CameraMode::Free)
	{
		// ��һ�˳�/����������Ĳ���

		// �����ƶ�
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

		// ��λ��������[-8.9f, 8.9f]��������
		// ��������
		XMFLOAT3 adjustedPos;
		XMStoreFloat3(&adjustedPos, XMVectorClamp(cam1st->GetPositionXM(), XMVectorSet(-8.9f, 0.0f, -8.9f, 0.0f), XMVectorReplicate(8.9f)));
		cam1st->SetPosition(adjustedPos);

		// ���ڵ�һ�˳�ģʽ�ƶ�����
		if (m_CameraMode == CameraMode::FirstPerson)
			m_WoodCrate.SetWorldMatrix(XMMatrixTranslation(adjustedPos.x, adjustedPos.y, adjustedPos.z));
		// ��Ұ��ת����ֹ��ʼ�Ĳ�ֵ�����µ�ͻȻ��ת
		cam1st->Pitch(mouseState.y * dt * 1.25f);
		cam1st->RotateY(mouseState.x * dt * 1.25f);
	}
	else if (m_CameraMode == CameraMode::ThirdPerson)
	{
		// �����˳�������Ĳ���

		cam3rd->SetTarget(m_WoodCrate.GetPosition());

		// ��������ת
		cam3rd->RotateX(mouseState.y * dt * 1.25f);
		cam3rd->RotateY(mouseState.x * dt * 1.25f);
		cam3rd->Approach(-mouseState.scrollWheelValue / 120 * 1.0f);
	}

	// ���¹۲����
	m_pCamera->UpdateViewMatrix();
	BasicEffect::Get().SetViewMatrix(m_pCamera->GetViewXM());
	BasicEffect::Get().SetEyePos(m_pCamera->GetPositionXM());

	// ���ù���ֵ
	m_pMouse->ResetScrollWheelValue();

	// �����ģʽ�л�
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
		// �������Ϸ���ʼ
		XMFLOAT3 pos = m_WoodCrate.GetPosition();
		XMFLOAT3 to = XMFLOAT3(0.0f, 0.0f, 1.0f);
		XMFLOAT3 up = XMFLOAT3(0.0f, 1.0f, 0.0f);
		pos.y += 3;
		cam1st->LookTo(pos, to, up);

		m_CameraMode = CameraMode::Free;
	}
	
	// �˳���������Ӧ�򴰿ڷ���������Ϣ
	if (keyState.IsKeyDown(Keyboard::Escape))
		SendMessage(MainWnd(), WM_DESTROY, 0, 0);
}

void GameApp::DrawScene()
{
	
	assert(m_pd3dImmediateContext);
	assert(m_pSwapChain);

	ImGui::Render();
	
	m_pd3dImmediateContext->ClearRenderTargetView(m_pRenderTargetView.Get(), m_bg);
	m_pd3dImmediateContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	/*
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
		
	}*/
	
	// ********************
	// 1. ���Ʋ�͸������
	//
	BasicEffect::Get().SetRenderDefault(m_pd3dImmediateContext.Get());


	for (auto& wall : m_Walls)
		wall.Draw(m_pd3dImmediateContext.Get(),BasicEffect::Get());
	m_Floor.Draw(m_pd3dImmediateContext.Get(), BasicEffect::Get());

	// ********************
	// 2. ����͸������
	//
	
	BasicEffect::Get().SetRenderAlphaBlend(m_pd3dImmediateContext.Get());

	// ��ʺ���΢̧��һ��߶�
	m_WoodCrate.SetWorldMatrix(XMMatrixTranslation(2.0f, 0.01f, 0.0f));
	m_WoodCrate.Draw(m_pd3dImmediateContext.Get(), BasicEffect::Get());
	m_WoodCrate.SetWorldMatrix(XMMatrixTranslation(-2.0f, 0.01f, 0.0f));
	m_WoodCrate.Draw(m_pd3dImmediateContext.Get(), BasicEffect::Get());
	// ��������ʺк��ٻ���ˮ��
	m_Water.Draw(m_pd3dImmediateContext.Get(), BasicEffect::Get());

	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	HR(m_pSwapChain->Present(0, 0));
}

bool GameApp::InitShader()
{

	if (BasicEffect::Get().InitAll(m_pd3dDevice.Get()))
		return true;
	return false;
}

bool GameApp::InitResource()
{
	m_CameraMode = CameraMode::FirstPerson;
	auto camera = std::shared_ptr<FirstPersonCamera>(new FirstPersonCamera);
	m_pCamera = camera;
	camera->SetViewPort(0.0f, 0.0f, (float)m_ClientWidth, (float)m_ClientHeight);
	camera->LookAt(XMFLOAT3(), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
	// ��ʼ�����ڴ��ڴ�С�䶯ʱ�޸ĵ�ֵ
	m_pCamera->SetFrustum(XM_PI / 3, AspectRatio(), 0.5f, 1000.0f);
	// ����Proj����
	BasicEffect::Get().SetProjMatrix(m_pCamera->GetProjXM());
#pragma region ��ʼ����Ϸ����
	// 
	ComPtr<ID3D11ShaderResourceView> texture;
	Material material{};
	material.ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	material.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	material.specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f);

	HR(CreateDDSTextureFromFile(m_pd3dDevice.Get(), L"Texture\\WireFence.dds", nullptr, texture.GetAddressOf()));
	m_WoodCrate.SetBuffer(m_pd3dDevice.Get(), Geometry::CreateBox<VertexPosNormalTex>());
	m_WoodCrate.SetTexture(texture.Get());
	m_WoodCrate.SetMaterial(material);

	// ��ʼ���ذ�
	HR(CreateDDSTextureFromFile(m_pd3dDevice.Get(), L"Texture\\floor.dds", nullptr, texture.ReleaseAndGetAddressOf()));
	m_Floor.SetBuffer(m_pd3dDevice.Get(),
		Geometry::CreatePlane<VertexPosNormalTex>(XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(20.0f, 20.0f), XMFLOAT2(5.0f, 5.0f)));
	m_Floor.SetTexture(texture.Get());
	m_Floor.SetMaterial(material);

	// ��ʼ��ǽ��
	m_Walls.resize(4);
	HR(CreateDDSTextureFromFile(m_pd3dDevice.Get(), L"Texture\\brick.dds", nullptr, texture.ReleaseAndGetAddressOf()));
	// �������ǽ���ĸ��������
	for (int i = 0; i < 4; ++i)
	{
		m_Walls[i].SetBuffer(m_pd3dDevice.Get(),
			Geometry::CreatePlane<VertexPosNormalTex>(XMFLOAT3(), XMFLOAT2(20.0f, 8.0f), XMFLOAT2(5.0f, 1.5f)));
		XMMATRIX world = XMMatrixRotationX(-XM_PIDIV2) * XMMatrixRotationY(XM_PIDIV2 * i)
			* XMMatrixTranslation(i % 2 ? -10.0f * (i - 2) : 0.0f, 3.0f, i % 2 == 0 ? -10.0f * (i - 1) : 0.0f);
		m_Walls[i].SetWorldMatrix(world);
		m_Walls[i].SetTexture(texture.Get());
		m_Walls[i].SetMaterial(material);
	}

	// ��ʼ��ˮ
	material.ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	material.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f);
	material.specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 32.0f);
	HR(CreateDDSTextureFromFile(m_pd3dDevice.Get(), L"Texture\\water.dds", nullptr, texture.ReleaseAndGetAddressOf()));
	m_Water.SetBuffer(m_pd3dDevice.Get(),
		Geometry::CreatePlane(XMFLOAT3(), XMFLOAT2(20.0f, 20.0f), XMFLOAT2(10.0f, 10.0f)));
	m_Water.SetTexture(texture.Get());
	m_Water.SetMaterial(material);
#pragma endregion

	
	
	// ******************
	// ��ʼ��Ĭ�Ϲ���
	auto& m_DirLight = m_dLight[0];
	auto& m_PointLight = m_pLight[0];
	auto& m_SpotLight = m_sLight[0];
	// �����
	m_DirLight.ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_DirLight.diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	m_DirLight.specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_DirLight.direction = XMFLOAT3(0.0f, -1.0f, 0.0f);
	// ���
	m_PointLight.position = XMFLOAT3(0.0f, 0.0f, -10.0f);
	m_PointLight.ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_PointLight.diffuse = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	m_PointLight.specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_PointLight.att = XMFLOAT3(0.0f, 0.1f, 0.0f);
	m_PointLight.range = 25.0f;
	// �۹��
	m_SpotLight.position = XMFLOAT3(0.0f, 0.0f, -5.0f);
	m_SpotLight.direction = XMFLOAT3(0.0f, 0.0f, 1.0f);
	m_SpotLight.ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_SpotLight.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_SpotLight.specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_SpotLight.att = XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_SpotLight.spot = 12.0f;
	m_SpotLight.range = 10000.0f;


	for (int i = 0; i < 10; i++)
	{
		BasicEffect::Get().SetDirLight(i, m_dLight[i]);
		BasicEffect::Get().SetPointLight(i, m_pLight[i]);
		BasicEffect::Get().SetSpotLight(i, m_sLight[i]);
	}

	return true;
}
