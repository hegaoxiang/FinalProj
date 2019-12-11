cbuffer CBChangeEveryFrame : register(b0)
{
    matrix g_View;
}
cbuffer CBChangeOnResize : register(b1)
{
    matrix g_Proj;
}
struct VertexPosColorIn
{
    float3 posL : POSITION;
    float4 color : COLOR;
};

struct VertexPosColorOut
{
    float4 posH : SV_POSITION;
    float4 color : COLOR;
};