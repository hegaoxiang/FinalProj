#pragma once
#include "Geometry.h"
#include "d3dApp.h"
#include "Camera.h"
#include "GameObejct.h"
#include "RenderStates.h"
#include "TextureRender.h"

class GameApp : public D3DApp
{
public:
	GameApp(HINSTANCE hInstance);
	~GameApp();

	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene();

	bool InitShader();
	bool InitResource();
	
private:

	CameraMode m_CameraMode = CameraMode::FirstPerson;
	std::shared_ptr<Camera> m_pCamera;
	
	GameObject m_WoodCrate;
	GameObject m_Floor;
	std::vector<GameObject> m_Walls;
	GameObject m_Water;
	GameObject m_Post;



	DirectionalLight m_dLight[10];
	PointLight m_pLight[10];
	SpotLight m_sLight[10];
	float m_bg[4] = { 0.0f, 0.0f, 1.0f, 1.0f };	// RGBA = (0,0,255,255)
	
	///
	/// 编辑界面
	///

	// 后期处理
	std::unique_ptr<TextureRender> m_pFullScreen;
	bool isPostProcess = false;

	// 包围盒线框

};

