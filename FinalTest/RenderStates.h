#pragma once
#include <wrl/client.h>
#include <d3d11.h>
class RenderStates
{
public:
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	static void InitAll(ID3D11Device* device);
	
	// ʹ��ComPtr�����ֹ��ͷ�
private:
	static bool IsInit();
public:
	static ComPtr<ID3D11RasterizerState> RSWireframe;   // ��դ����״̬���߿�ģʽ
	static ComPtr<ID3D11RasterizerState> RSNoCull;      // ��դ����״̬���ޱ���ü�ģʽ

	static ComPtr<ID3D11SamplerState> SSLinearWrap;         // ������״̬�����Թ���
	static ComPtr<ID3D11SamplerState> SSAnistropicWrap;     // ������״̬���������Թ���

	static ComPtr<ID3D11BlendState> BSNoColorWrite;     // ���״̬����д����ɫ
	static ComPtr<ID3D11BlendState> BSTransparent;      // ���״̬��͸�����
	static ComPtr<ID3D11BlendState> BSAlphaToCoverage;  // ���״̬��Alpha-To-Coverage
};

