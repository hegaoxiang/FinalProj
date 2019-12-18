#include "GameApp.h"
#include "d3dUtil.h"
#include "DXTrace.h"
#include "BasicEffect.h"
#include "PostEffect.h"
#include "WireEffect.h"
#include "Collision.h"
#include <iostream>
// serialize
#include <rapidjson/filewritestream.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/document.h>
using namespace rapidjson;

using namespace DirectX;

ID3D11Device* g_device;
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
	g_device = m_pd3dDevice.Get();
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
		WireEffect::Get().SetProjMatrix(m_pCamera->GetProjXM());
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
	m_MouseTracker.Update(mouseState);

	auto relaX  = mouseState.x - lastMouseState.x;
	auto relaY = mouseState.y - lastMouseState.y;
	


	auto keyState = m_pKeyboard->GetState();
	m_KeyboardTracker.Update(keyState);
	
	
	bool w = keyState.IsKeyDown(Keyboard::LeftControl);

	
	ImGui::Begin("Editor");   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
	ImGui::ColorEdit4("colorbg", (float*)&m_bg);
	ImGui::Checkbox("PostProcess", & isPostProcess);
	ImGui::Checkbox("button", &w);
	//ImGui::SliderInt("textureIdx", &i, 0, 5);

	ImGui::End();

	static Mouse::State noOperation = {};
	if (w)
	{
		m_pMouse->SetVisible(false);
	}else
	{
		m_pMouse->SetVisible(true);	
		relaX = noOperation.x;
		relaY = noOperation.y;
	}
	
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
		cam1st->Pitch(relaY * dt * 1.25f);
		cam1st->RotateY(relaX * dt * 1.25f);

		
	}
	else if (m_CameraMode == CameraMode::ThirdPerson)
	{
		// 第三人称摄像机的操作

		cam3rd->SetTarget(m_WoodCrate.GetPosition());

		// 绕物体旋转
		cam3rd->RotateX(relaY * dt * 1.25f);
		cam3rd->RotateY(relaX * dt * 1.25f);
		cam3rd->Approach(-mouseState.scrollWheelValue / 120 * 1.0f);
	}

	// 更新观察矩阵
	m_pCamera->UpdateViewMatrix();
	BasicEffect::Get().SetViewMatrix(m_pCamera->GetViewXM());
	BasicEffect::Get().SetEyePos(m_pCamera->GetPositionXM());
	WireEffect::Get().SetViewMatrix(m_pCamera->GetViewXM());

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
	
	// Pick
	if (keyState.IsKeyDown(Keyboard::LeftAlt))
	{
		auto ray = Ray::ScreenToRay(*m_pCamera, (float)mouseState.x, (float)mouseState.y );
		if (ray.Hit(m_WoodCrate.GetLocalBoundingBox()))
		{
			m_WoodCrate.isHit = true;
		}
	}

	// 退出程序，这里应向窗口发送销毁信息
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
	
	if(isPostProcess)
		m_pFullScreen->Begin(m_pd3dImmediateContext.Get());
	// ********************
	// 1. 绘制不透明对象
	//
	BasicEffect::Get().SetRenderDefault(m_pd3dImmediateContext.Get());

	
	for (auto& item : m_staticScene.GetObjects())
		item->Draw(m_pd3dImmediateContext.Get(),&BasicEffect::Get());

	// ********************
	// 2. 绘制透明对象
	//
	
	BasicEffect::Get().SetRenderAlphaBlend(m_pd3dImmediateContext.Get());

	// 篱笆盒稍微抬起一点高度
	//m_WoodCrate.SetWorldMatrix(XMMatrixTranslation(2.0f, 0.01f, 0.0f));
	m_WoodCrate.Draw(m_pd3dImmediateContext.Get(), &BasicEffect::Get());
	if(m_WoodCrate.isHit)
	{
		m_WoodCrate.isHit = false;
		WireEffect::Get().SetWireRender(m_pd3dImmediateContext.Get());
		auto wire = Collision::CreateBoundingBox(m_WoodCrate.GetLocalBoundingBox(), XMFLOAT4(0.5, 0.1, 0.1, 1.0));

		ComPtr<ID3D11Buffer> m_pVertexBuffer;
		ComPtr<ID3D11Buffer> m_pIndexBuffer;

		auto m_VertexStride = sizeof(VertexPosColor);
		// 设置顶点缓冲区描述
		D3D11_BUFFER_DESC vbd;
		ZeroMemory(&vbd, sizeof(vbd));
		vbd.Usage = D3D11_USAGE_IMMUTABLE;
		vbd.ByteWidth = m_VertexStride * wire.vertexVec.size();
		vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbd.CPUAccessFlags = 0;
		// 新建顶点缓冲区
		D3D11_SUBRESOURCE_DATA InitData;
		ZeroMemory(&InitData, sizeof(InitData));
		InitData.pSysMem = wire.vertexVec.data();
		HR(m_pd3dDevice->CreateBuffer(&vbd, &InitData, m_pVertexBuffer.GetAddressOf()));

		UINT m_IndexCount = (UINT)wire.indexVec.size();
		// 设置索引缓冲区描述
		D3D11_BUFFER_DESC ibd;
		ZeroMemory(&ibd, sizeof(ibd));
		ibd.Usage = D3D11_USAGE_IMMUTABLE;
		ibd.ByteWidth = sizeof(WORD) * m_IndexCount;
		ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibd.CPUAccessFlags = 0;
		// 新建索引缓冲区
		InitData.pSysMem = wire.indexVec.data();
		HR(m_pd3dDevice->CreateBuffer(&ibd, &InitData, m_pIndexBuffer.GetAddressOf()));
		// 输入装配阶段的索引缓冲区设置

		D3D11SetDebugObjectName(m_pIndexBuffer.Get(), "index");
		D3D11SetDebugObjectName(m_pVertexBuffer.Get(), "vertex");

		UINT offset = 0;
		m_pd3dImmediateContext->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), &m_VertexStride, &offset);
		m_pd3dImmediateContext->IASetIndexBuffer(m_pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

		WireEffect::Get().Apply(m_pd3dImmediateContext.Get());
		m_pd3dImmediateContext->DrawIndexed(m_IndexCount, 0, 0);

		BasicEffect::Get().SetRenderAlphaBlend(m_pd3dImmediateContext.Get());
	}
	

	//m_WoodCrate.SetWorldMatrix(XMMatrixTranslation(-2.0f, 0.01f, 0.0f));
	//m_WoodCrate.Draw(m_pd3dImmediateContext.Get(), &BasicEffect::Get());
	// 绘制了篱笆盒后再绘制水面
	m_Water.Draw(m_pd3dImmediateContext.Get(), &BasicEffect::Get());

	if (isPostProcess)
	{
		m_pFullScreen->End(m_pd3dImmediateContext.Get());

		PostEffect::Get().SetRenderDefault(m_pd3dImmediateContext.Get());

		m_Post.SetTexture(m_pFullScreen->GetOutputTexture());
		m_Post.Draw(m_pd3dImmediateContext.Get(), &PostEffect::Get());

		PostEffect::Get().SetTexture(nullptr);
		PostEffect::Get().Apply(m_pd3dImmediateContext.Get());
	}

	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	HR(m_pSwapChain->Present(0, 0));
}

bool GameApp::InitShader()
{

	if (!BasicEffect::Get().InitAll(m_pd3dDevice.Get()))
		return false;

	if (!PostEffect::Get().InitAll(m_pd3dDevice.Get()))
		return false;
	
	if (!WireEffect::Get().InitAll(m_pd3dDevice.Get()))
		return false;

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
	// 更新Proj矩阵
	BasicEffect::Get().SetProjMatrix(m_pCamera->GetProjXM());
	WireEffect::Get().SetProjMatrix(m_pCamera->GetProjXM());
#pragma region 初始化游戏对象
	// 
	
	Material material{};
	material.ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	material.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	material.specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f);

	

	m_WoodCrate.Init();
	//HR(CreateDDSTextureFromFile(m_pd3dDevice.Get(), L"Texture\\WireFence.dds", nullptr, texture.GetAddressOf()));
	m_WoodCrate.SetBuffer(m_pd3dDevice.Get(), Geometry::CreateBox<VertexPosNormalTex>());
	m_WoodCrate.SetTexture(m_pd3dDevice.Get(), L"Texture\\WireFence.dds");
	m_WoodCrate.SetMaterial(material);
	#if 0
	
	GameObject* floor = new GameObject();
	// 初始化地板
	floor->Init();
	//HR(CreateDDSTextureFromFile(m_pd3dDevice.Get(), L"Texture\\floor.dds", nullptr, texture.ReleaseAndGetAddressOf()));
	floor->SetBuffer(m_pd3dDevice.Get(),
		Geometry::CreatePlane<VertexPosNormalTex>(XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT2(20.0f, 20.0f), XMFLOAT2(5.0f, 5.0f)));
	floor->SetBufferAttr(1, 1, { 0, -1, 0, 20, 20, 5, 5 });
	floor->SetTexture(m_pd3dDevice.Get(), L"Texture\\floor.dds");
	floor->SetMaterial(material);
	floor->SetName("floor");
	m_staticScene.AddStaticObject(StrongGameObjectPtr(floor));
	// test

	std::vector<GameObject*> walls;
	// 初始化墙体
	walls.resize(4);
	//HR(CreateDDSTextureFromFile(m_pd3dDevice.Get(), L"Texture\\brick.dds", nullptr, texture.ReleaseAndGetAddressOf()));
	// 这里控制墙体四个面的生成
	for (int i = 0; i < 4; ++i)
	{		
		walls[i] = new GameObject();
		walls[i]->Init();
		walls[i]->SetBuffer(m_pd3dDevice.Get(),
			Geometry::CreatePlane<VertexPosNormalTex>(XMFLOAT3(), XMFLOAT2(20.0f, 8.0f), XMFLOAT2(5.0f, 1.5f)));
		walls[i]->SetBufferAttr(1, 1, { 0,0,0,20,8,5,1.5 });
		XMMATRIX world = XMMatrixRotationX(-XM_PIDIV2) * XMMatrixRotationY(XM_PIDIV2 * i)
			* XMMatrixTranslation(i % 2 ? -10.0f * (i - 2) : 0.0f, 3.0f, i % 2 == 0 ? -10.0f * (i - 1) : 0.0f);
		walls[i]->SetWorldMatrix(world);
		walls[i]->SetTexture(m_pd3dDevice.Get(), L"Texture\\brick.dds");
		walls[i]->SetMaterial(material);
		walls[i]->SetName("wall");
	

		m_staticScene.AddStaticObject(StrongGameObjectPtr(walls[i]));		
	}
	
	if (true)
{
	StringBuffer s;
		PrettyWriter<StringBuffer> write(s);
		m_staticScene.Serialize(write);
		auto a =std::string(s.GetString());
		Document d;
		assert(kParseErrorNone == d.Parse(s.GetString()).HasParseError());
	
		FILE* fp;
		fopen_s(&fp, "staticScene.json", "wb");
		assert(fp != nullptr);
	
		char *writeBuffer = new char[65536];
		FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));
			
		PrettyWriter<FileWriteStream> fileWriter(os);
		d.Accept(fileWriter);
		
		delete[] writeBuffer;
		fclose(fp);
}
#endif

	m_staticScene.AntiSerialize("staticScene.json");
	
	// 初始化水
	m_Water.Init();
	material.ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	material.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f);
	material.specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 32.0f);
	//HR(CreateDDSTextureFromFile(m_pd3dDevice.Get(), L"Texture\\water.dds", nullptr, texture.ReleaseAndGetAddressOf()));
	m_Water.SetBuffer(m_pd3dDevice.Get(),
		Geometry::CreatePlane(XMFLOAT3(), XMFLOAT2(20.0f, 20.0f), XMFLOAT2(10.0f, 10.0f)));
	m_Water.SetTexture(m_pd3dDevice.Get(), L"Texture\\water.dds");
	m_Water.SetMaterial(material);
#pragma endregion

#pragma region 初始化光照
	// ******************
	// 初始化默认光照
	auto& m_DirLight = m_dLight[0];
	auto& m_PointLight = m_pLight[0];
	auto& m_SpotLight = m_sLight[0];
	// 方向光
	m_DirLight.ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_DirLight.diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	m_DirLight.specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_DirLight.direction = XMFLOAT3(0.0f, -1.0f, 0.0f);
	// 点光
	m_PointLight.position = XMFLOAT3(0.0f, 0.0f, -10.0f);
	m_PointLight.ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_PointLight.diffuse = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	m_PointLight.specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_PointLight.att = XMFLOAT3(0.0f, 0.1f, 0.0f);
	m_PointLight.range = 25.0f;
	// 聚光灯
	m_SpotLight.position = XMFLOAT3(0.0f, 0.0f, -5.0f);
	m_SpotLight.direction = XMFLOAT3(0.0f, 0.0f, 1.0f);
	m_SpotLight.ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_SpotLight.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_SpotLight.specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_SpotLight.att = XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_SpotLight.spot = 12.0f;
	m_SpotLight.range = 10000.0f;

#pragma endregion
	m_Post.Init();
	m_Post.SetBuffer(m_pd3dDevice.Get(), Geometry::Create2DShow());
	for (int i = 0; i < 10; i++)
	{
		BasicEffect::Get().SetDirLight(i, m_dLight[i]);
		BasicEffect::Get().SetPointLight(i, m_pLight[i]);
		BasicEffect::Get().SetSpotLight(i, m_sLight[i]);
	}

	m_pFullScreen = std::make_unique<TextureRender>(m_pd3dDevice.Get(), m_ScreenViewport.Width, m_ScreenViewport.Height) ;
	return true;
}
