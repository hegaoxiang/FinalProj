#include"Triangle.hlsli"

VertexOut VS(VertexPosTexNorIn vIn)
{
    VertexOut vOut;

    matrix viewProj = mul(g_View, g_Proj);

    float4 posW = mul(float4(vIn.posL, 1.0f), g_World);
    vOut.normalW = mul(vIn.normalL, (float3x3) g_WorldInvTranspose);

    // 若当前在绘制反射物体，先进行反射操作
    //[flatten]
    //if (g_IsReflection)
    //{
    //    posW = mul(posW, g_Reflection);
    //    vOut.normalW = mul(vOut.normalW, (float3x3) g_Reflection);
    //}

    vOut.posH = mul(posW, viewProj);

    vOut.posW = posW.xyz;


    vOut.tex = vIn.tex;

    return vOut;
}