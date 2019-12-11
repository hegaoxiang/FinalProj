#include"WireFrame.hlsli"
VertexPosColorOut VS( VertexPosColorIn vIn)
{
    VertexPosColorOut vOut;
    matrix viewProj = mul(g_View, g_Proj);
    vOut.posH = mul(float4(vIn.posL, 1.0f), viewProj);
    vOut.color = vIn.color;
	return vOut;
}