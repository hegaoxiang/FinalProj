#include "Triangle.hlsli"

// ÏñËØ×ÅÉ«Æ÷
float4 PS(VertexOut pIn) : SV_Target
{
    return gTex.Sample(gSamLinear, pIn.tex);
}