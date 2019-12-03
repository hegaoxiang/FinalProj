#include"Triangle.hlsli"

float4 PS(VertexOut pIn) : SV_TARGET
{
    float4 texColor = g_Tex.Sample(g_SamLinear, pIn.tex);
    clip(texColor.a - 0.1f);
    
    // 标准化法向量
    pIn.normalW = normalize(pIn.normalW);
    
    // 指向眼睛的向量
    float3 toEye = normalize(g_EyePosW - pIn.posW);
  
    // 初始化为0 
    float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 spec = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 A = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 D = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 S = float4(0.0f, 0.0f, 0.0f, 0.0f);
    int i;


	// 强制展开循环以减少指令数
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
    

     // 若当前在绘制反射物体，需要对光照进行反射矩阵变换
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
    // 若当前在绘制反射物体，需要对光照进行反射矩阵变换
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