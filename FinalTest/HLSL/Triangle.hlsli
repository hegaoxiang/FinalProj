
Texture2D gTex : register(t0);
SamplerState gSamLinear : register(s0);

cbuffer CBChangeEveryDraw : register(b0)
{
    matrix gWorld;
}
cbuffer CBChangeEveryFrame : register(b1)
{
	matrix gView;
}
cbuffer CBChangeOnResize : register(b2)
{
    matrix gProj;
}

struct VertexTexIn
{
	float3 posL : POSITION;
    //float3 normalL : NORMAL;
    
    float2 tex : TEXCOORD;
};

struct VertexOut
{
	float4 posH : SV_POSITION;
    float3 posW : POSITIONT;
   // float3 normalW : NORMAL;
    float2 tex : TEXCOORD;
	
};