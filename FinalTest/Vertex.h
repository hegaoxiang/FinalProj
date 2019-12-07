#pragma once
#include <d3d11_1.h>
#include <DirectXMath.h>

struct VertexPos
{
	VertexPos() = default;

	VertexPos(const VertexPos&) = default;
	VertexPos& operator=(const VertexPos&) = default;

	VertexPos(VertexPos&&) = default;
	VertexPos& operator=(VertexPos&&) = default;

	constexpr VertexPos(const DirectX::XMFLOAT3& _pos) : pos(_pos) {}

	DirectX::XMFLOAT3 pos;
	static const D3D11_INPUT_ELEMENT_DESC inputLayout[1];
};

struct VertexPosColor
{
	VertexPosColor() = default;

	VertexPosColor(const VertexPosColor&) = default;
	VertexPosColor& operator=(const VertexPosColor&) = default;

	VertexPosColor(VertexPosColor&&) = default;
	VertexPosColor& operator=(VertexPosColor&&) = default;

	constexpr VertexPosColor(const DirectX::XMFLOAT3& _pos, const DirectX::XMFLOAT4& _color) :
		pos(_pos), color(_color) {}

	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT4 color;
	static const D3D11_INPUT_ELEMENT_DESC inputLayout[2];
};

struct VertexPosTex
{
	VertexPosTex() = default;

	VertexPosTex(const VertexPosTex&) = default;
	VertexPosTex& operator=(const VertexPosTex&) = default;

	VertexPosTex(VertexPosTex&&) = default;
	VertexPosTex& operator=(VertexPosTex&&) = default;

	constexpr VertexPosTex(const DirectX::XMFLOAT3& _pos, const DirectX::XMFLOAT2& _tex) :
		pos(_pos), tex(_tex) {}

	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT2 tex;
	static const D3D11_INPUT_ELEMENT_DESC inputLayout[2];
};

struct VertexPosSize
{
	VertexPosSize() = default;

	VertexPosSize(const VertexPosSize&) = default;
	VertexPosSize& operator=(const VertexPosSize&) = default;

	VertexPosSize(VertexPosSize&&) = default;
	VertexPosSize& operator=(VertexPosSize&&) = default;

	constexpr VertexPosSize(const DirectX::XMFLOAT3& _pos, const DirectX::XMFLOAT2& _size) :
		pos(_pos), size(_size) {}

	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT2 size;
	static const D3D11_INPUT_ELEMENT_DESC inputLayout[2];
};

struct VertexPosNormalColor
{
	VertexPosNormalColor() = default;

	VertexPosNormalColor(const VertexPosNormalColor&) = default;
	VertexPosNormalColor& operator=(const VertexPosNormalColor&) = default;

	VertexPosNormalColor(VertexPosNormalColor&&) = default;
	VertexPosNormalColor& operator=(VertexPosNormalColor&&) = default;

	constexpr VertexPosNormalColor(const DirectX::XMFLOAT3& _pos, const DirectX::XMFLOAT3& _normal,
		const DirectX::XMFLOAT4& _color) :
		pos(_pos), normal(_normal), color(_color) {}

	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT4 color;
	static const D3D11_INPUT_ELEMENT_DESC inputLayout[3];
};

// 物体表面材质
struct Material
{
	Material() { memset(this, 0, sizeof(Material)); }

	DirectX::XMFLOAT4 ambient;
	DirectX::XMFLOAT4 diffuse;
	DirectX::XMFLOAT4 specular; // w = 镜面反射强度
	DirectX::XMFLOAT4 reflect;
};

struct VertexPosNormalTex
{
	VertexPosNormalTex() = default;

	VertexPosNormalTex(const VertexPosNormalTex&) = default;
	VertexPosNormalTex& operator=(const VertexPosNormalTex&) = default;

	VertexPosNormalTex(VertexPosNormalTex&&) = default;
	VertexPosNormalTex& operator=(VertexPosNormalTex&&) = default;

	constexpr VertexPosNormalTex(const DirectX::XMFLOAT3& _pos, const DirectX::XMFLOAT3& _normal,
		const DirectX::XMFLOAT2& _tex) :
		pos(_pos), normal(_normal), tex(_tex) {}

	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT2 tex;
	static const D3D11_INPUT_ELEMENT_DESC inputLayout[3];
};

struct VertexPosNormalTangentTex
{
	VertexPosNormalTangentTex() = default;

	VertexPosNormalTangentTex(const VertexPosNormalTangentTex&) = default;
	VertexPosNormalTangentTex& operator=(const VertexPosNormalTangentTex&) = default;

	VertexPosNormalTangentTex(VertexPosNormalTangentTex&&) = default;
	VertexPosNormalTangentTex& operator=(VertexPosNormalTangentTex&&) = default;

	constexpr VertexPosNormalTangentTex(const DirectX::XMFLOAT3& _pos, const DirectX::XMFLOAT3& _normal,
		const DirectX::XMFLOAT4& _tangent, const DirectX::XMFLOAT2& _tex) :
		pos(_pos), normal(_normal), tangent(_tangent), tex(_tex) {}

	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT4 tangent;
	DirectX::XMFLOAT2 tex;
	static const D3D11_INPUT_ELEMENT_DESC inputLayout[4];
};


struct DirectionalLight
{
	DirectionalLight() { memset(this, 0, sizeof(DirectionalLight)); }

	DirectX::XMFLOAT4 ambient;
	DirectX::XMFLOAT4 diffuse;
	DirectX::XMFLOAT4 specular;
	DirectX::XMFLOAT3 direction;
	float Pad; // 最后用一个浮点数填充使得该结构体大小满足16的倍数，便于我们以后在HLSL设置数组
};

// 点光
struct PointLight
{
	PointLight() { memset(this, 0, sizeof(PointLight)); }

	DirectX::XMFLOAT4 ambient;
	DirectX::XMFLOAT4 diffuse;
	DirectX::XMFLOAT4 specular;

	// 打包成4D向量: (Position, Range)
	DirectX::XMFLOAT3 position;
	float range;

	// 打包成4D向量: (A0, A1, A2, Pad)
	DirectX::XMFLOAT3 att;
	float Pad; // 最后用一个浮点数填充使得该结构体大小满足16的倍数，便于我们以后在HLSL设置数组
};

struct SpotLight
{
	SpotLight() { memset(this, 0, sizeof(SpotLight)); }

	DirectX::XMFLOAT4 ambient;
	DirectX::XMFLOAT4 diffuse;
	DirectX::XMFLOAT4 specular;

	// 打包成4D向量: (Position, Range)
	DirectX::XMFLOAT3 position;
	float range;

	// 打包成4D向量: (Direction, Spot)
	DirectX::XMFLOAT3 direction;
	float spot;

	// 打包成4D向量: (Att, Pad)
	DirectX::XMFLOAT3 att;
	float Pad; // 最后用一个浮点数填充使得该结构体大小满足16的倍数，便于我们以后在HLSL设置数组
};

