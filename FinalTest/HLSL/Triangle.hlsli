#include "LightHelper.hlsli"

Texture2D g_Tex : register(t0);
SamplerState g_SamLinear : register(s0);


cbuffer CBChangeEveryDraw : register(b0)
{
    matrix g_World;
    matrix g_WorldInvTranspose;
	Material g_Material;
}
cbuffer CBDrawingStates : register(b4)
{
    int g_IsReflection;
    float3 g_Pad1;
}

cbuffer CBChangeEveryFrame : register(b1)
{
	matrix g_View;
    float3 g_EyePosW;
}
cbuffer CBChangeOnResize : register(b2)
{
    matrix g_Proj;
}
cbuffer CBChangeOnLightChange : register(b3)
{
    
    DirectionalLight g_DirectLight[10];
    PointLight g_PointLight[10];
    SpotLight g_SpotLight[10];
    int g_NumDirectLight;
    int g_NumPointLight;
    int g_NumSpotLight;
    float pad;
}

struct VertexPosTexNorIn
{
    float3 posL : POSITION;
    float3 normalL : NORMAL;
    
    float2 tex : TEXCOORD;
};


struct VertexPosTexIn
{
	float3 posL : POSITION;
    float2 tex : TEXCOORD;
};

struct VertexOut
{
	float4 posH : SV_POSITION;
    float3 posW : POSITIONT;
    float3 normalW : NORMAL;
    float2 tex : TEXCOORD;
	
};