#include "Triangle.hlsli"

// ¶¥µã×ÅÉ«Æ÷
VertexOut VS(VertexTexIn vIn)
{
	VertexOut vOut;
	vOut.posH = float4(vIn.posL, 1.0f);
    vOut.posH = mul(vOut.posH, gWorld);
    
    vOut.posW = vOut.posH;
    
    vOut.posH = mul(vOut.posH, gView);
    vOut.posH = mul(vOut.posH, gProj);
    
    vOut.tex = vIn.tex;
    
	return vOut;
}