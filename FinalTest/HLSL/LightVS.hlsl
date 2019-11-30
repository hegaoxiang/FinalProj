#include"Triangle.hlsli"

VertexOut VS(VertexPosTexNorIn vIn)
{
 /*   VertexOut vOut;
    vOut.posH = float4(vIn.posL, 1.0f);
    vOut.posH = mul(vOut.posH, g_World);
    
    vOut.posW = mul(vIn.normalL, (float3x3) g_WorldInvTranspose);;
    
    vOut.posH = mul(vOut.posH, g_View);
    vOut.posH = mul(vOut.posH, g_Proj);
    
    vOut.tex = vIn.tex;
    vOut.normalW = mul(vIn.normalL, (float3x3) g_WorldInvTranspose);
    
    return vOut;*/
    VertexOut vOut;

    matrix viewProj = mul(g_View, g_Proj);

    float4 posW = mul(float4(vIn.posL, 1.0f), g_World);



    vOut.posH = mul(posW, viewProj);

    vOut.posW = posW.xyz;

    vOut.normalW = mul(vIn.normalL, (float3x3) g_WorldInvTranspose);

    vOut.tex = vIn.tex;

    return vOut;
}