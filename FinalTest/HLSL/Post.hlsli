Texture2D g_Tex : register(t0);
SamplerState g_Sample : register(s0);

struct VertexPosTexIn
{
    float3 posL : POSITION;
    float2 tex : TEXCOORD;
};

struct VertexPosTexOut
{
    float4 posH : SV_Position;
    float2 tex : TEXCOORD;
};