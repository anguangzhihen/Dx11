#include "Shapes.fx"

VertexOut VS(VertexIn pIn)
{
	VertexOut pOut;
	pOut.PosH = mul(float4(pIn.PosL, 1.0f), wvp);

	pOut.Color = pIn.Color;
	return pOut;
}