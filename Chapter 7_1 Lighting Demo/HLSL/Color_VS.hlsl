// Color_VS.hlsl¶¥µã×ÅÉ«Æ÷
#include "Color.fx"

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	vout.PosW = mul(float4(vin.PosL, 1.0f), gWorld).xyz;
	vout.NormalW = mul(vin.NormalL, (float3x3)gWorldInvTranspose);

	vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);

	return vout;
}