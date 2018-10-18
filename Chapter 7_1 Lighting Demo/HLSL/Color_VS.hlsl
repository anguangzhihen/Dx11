// Color_VS.hlsl顶点着色器
#include "Color.fx"

VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	// 世界坐标和世界法线，法线通过World逆转置矩阵进行转换
	vout.PosW = mul(float4(vin.PosL, 1.0f), gWorld).xyz;
	vout.NormalW = mul(vin.NormalL, (float3x3)gWorldInvTranspose);

	vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);

	return vout;
}