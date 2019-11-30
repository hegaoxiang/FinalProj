#include"Triangle.hlsli"

float4 PS(VertexOut pIn) : SV_TARGET
{
    float4 texColor = g_Tex.Sample(g_SamLinear, pIn.tex);
    clip(texColor.a - 0.1f);
    //return texColor;
    
    // ��׼��������
    pIn.normalW = normalize(pIn.normalW);
    
    // ָ���۾�������
    float3 toEye = normalize(g_EyePosW - pIn.posW);
  
    // ��ʼ��Ϊ0 
    float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 spec = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 A = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 D = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 S = float4(0.0f, 0.0f, 0.0f, 0.0f);
    int i;


	// ǿ��չ��ѭ���Լ���ָ����
	[unroll]
    for (i = 0; i < g_NumDirectLight; ++i)
    {
        ComputeDirectionalLight(g_Material, g_DirectLight[i], pIn.normalW, toEye, A, D, S);
        ambient += A;
        diffuse += D;
        spec += S;
    }
    

	[unroll]
    for (i = 0; i < g_NumPointLight; ++i)
    {
        ComputePointLight(g_Material, g_PointLight[i], pIn.posW, pIn.normalW, toEye, A, D, S);
        ambient += A;
        diffuse += D;
        spec += S;
    }

	[unroll]
    for (i = 0; i < g_NumSpotLight; ++i)
    {
        ComputeSpotLight(g_Material, g_SpotLight[i], pIn.posW, pIn.normalW, toEye, A, D, S);
        ambient += A;
        diffuse += D;
        spec += S;
    }

    float4 litColor = texColor * (ambient + diffuse) + spec;
    litColor.a = texColor.a * g_Material.Diffuse.a;
    return litColor;
}