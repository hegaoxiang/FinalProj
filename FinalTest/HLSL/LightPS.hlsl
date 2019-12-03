#include"Triangle.hlsli"

float4 PS(VertexOut pIn) : SV_TARGET
{
    float4 texColor = g_Tex.Sample(g_SamLinear, pIn.tex);
    clip(texColor.a - 0.1f);
    
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
        DirectionalLight dirLight = g_DirectLight[i];
       // [flatten]
       // if (g_IsReflection)
       // {
       //     dirLight.Direction = mul(dirLight.Direction, (float3x3) (g_Reflection));
       // }
        ComputeDirectionalLight(g_Material, dirLight, pIn.normalW, toEye, A, D, S);
        ambient += A;
        diffuse += D;
        spec += S;
    }
    

     // ����ǰ�ڻ��Ʒ������壬��Ҫ�Թ��ս��з������任
    PointLight pointLight;
	[unroll]
    for (i = 0; i < g_NumPointLight; ++i)
    {
        pointLight = g_PointLight[i];
        //[flatten]
        //if (g_IsReflection)
        //{
        //    pointLight.Position = (float3) mul(float4(pointLight.Position, 1.0f), g_Reflection);
        //}
        ComputePointLight(g_Material, pointLight, pIn.posW, pIn.normalW, toEye, A, D, S);
        ambient += A;
        diffuse += D;
        spec += S;
    }

    
    SpotLight spotLight;
    // ����ǰ�ڻ��Ʒ������壬��Ҫ�Թ��ս��з������任
	[unroll]
    for (i = 0; i < g_NumSpotLight; ++i)
    {
        
        spotLight = g_SpotLight[i];
        //[flatten]
        //if (g_IsReflection)
        //{
        //    spotLight.Position = (float3) mul(float4(spotLight.Position, 1.0f), g_Reflection);
        //    spotLight.Direction = mul(spotLight.Direction, (float3x3) g_Reflection);
        //}
        
        ComputeSpotLight(g_Material, spotLight, pIn.posW, pIn.normalW, toEye, A, D, S);
        ambient += A;
        diffuse += D;
        spec += S;
    }

    float4 litColor = texColor * (ambient + diffuse) + spec;
    litColor.a = texColor.a * g_Material.Diffuse.a;
    return litColor;
}