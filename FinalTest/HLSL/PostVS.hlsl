#include"Post.hlsli"

VertexPosTexOut VS( VertexPosTexIn vIn )
{
    VertexPosTexOut vOut;
    vOut.posH = float4(vIn.posL, 1.0);
    
    vOut.tex = vIn.tex;
	return vOut;
}