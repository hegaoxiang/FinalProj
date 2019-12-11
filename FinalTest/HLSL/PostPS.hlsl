#include"Post.hlsli"

float4 PS(VertexPosTexOut pIn) : SV_TARGET
{
    float4 texColor = g_Tex.Sample(g_Sample, pIn.tex);
    
    return float4(float3((1.0 - texColor).xyz), 1.0f);
}