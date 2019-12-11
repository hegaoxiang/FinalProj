#include"WireFrame.hlsli"
float4 PS(VertexPosColorOut pIn) : SV_TARGET
{
	return pIn.color;
}