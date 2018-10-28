#include "Shapes.fx"

VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	// 世界坐标和世界法线，法线通过World逆转置矩阵进行转换
	vout.PosW = mul(float4(vin.PosL, 1.0f), gWorld).xyz;
	vout.NormalW = mul(vin.NormalL, (float3x3)gWorldInvTranspose);

	vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);

	// 使用矩阵变换当前的Tex值
	vout.Tex = mul(float4(vin.Tex, 0.0f, 1.0f), gTexTransform).xy;

	//// Shader
	//vout.Tex = mul(float4(vin.Tex - float2(0.5f, 0.5f), 0.0f, 1.0f), gTexTransform).xy;	// 练习8.4

	return vout;
}