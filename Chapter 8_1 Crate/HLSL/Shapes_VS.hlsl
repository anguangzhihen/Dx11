#include "Shapes.fx"

VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	// ������������編�ߣ�����ͨ��World��ת�þ������ת��
	vout.PosW = mul(float4(vin.PosL, 1.0f), gWorld).xyz;
	vout.NormalW = mul(vin.NormalL, (float3x3)gWorldInvTranspose);

	vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);

	// ʹ�þ���任��ǰ��Texֵ
	vout.Tex = mul(float4(vin.Tex, 0.0f, 1.0f), gTexTransform).xy;

	//// Shader
	//vout.Tex = mul(float4(vin.Tex - float2(0.5f, 0.5f), 0.0f, 1.0f), gTexTransform).xy;	// ��ϰ8.4

	return vout;
}