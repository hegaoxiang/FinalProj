#pragma once
#include "Geometry.h"
#include "d3dApp.h"
#include "Camera.h"
#include "GameObejct.h"
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
	ComPtr<ID3D11VertexShader> m_pVertexShader;
	ComPtr<ID3D11PixelShader> m_pPixelShader;
	ComPtr<ID3D11InputLayout> m_pVertexLayout;
	ComPtr<ID3D11Buffer> m_pVertexBuffer;
	ComPtr<ID3D11Buffer> m_pConstantBuffers[3];
	ComPtr<ID3D11Buffer> m_pIndexBuffer;
	ComPtr<ID3D11SamplerState> m_pSamplerState;

	ComPtr<ID3D11ShaderResourceView> m_pWoodCrate;
	ComPtr<ID3D11ShaderResourceView> m_pTemp[6];

	std::unique_ptr<FirstPersonCamera> m_pCamera;
	
	GameObject m_WoodCrate;
	GameObject m_Floor;
	std::vector<GameObject> m_Walls;
	float m_bg[4] = { 0.0f, 0.0f, 1.0f, 1.0f };	// RGBA = (0,0,255,255)
};

