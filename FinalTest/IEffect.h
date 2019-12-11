#pragma once
#include "CBuffer.h"
#include <memory>
#include <d3dcompiler.h>
#include <vector>
#include "RenderStates.h"
class IEffect
{
public:
	// ʹ��ģ�����(C++11)��������
	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	IEffect() = default;

	// ��֧�ָ��ƹ���
	IEffect(const IEffect&) = delete;
	IEffect& operator=(const IEffect&) = delete;

	// ����ת��
	IEffect(IEffect && moveFrom) = default;
	IEffect& operator=(IEffect && moveFrom) = default;

	virtual ~IEffect() = default;

	// ���²��󶨳���������
	virtual void Apply(ID3D11DeviceContext * deviceContext) = 0;
};

