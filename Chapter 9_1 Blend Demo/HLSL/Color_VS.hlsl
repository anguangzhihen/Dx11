// Color_VS.hlsl������ɫ��
#include "Color.fx"

VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	// ������������編�ߣ�����ͨ��World��ת�þ������ת��
	vout.PosW = mul(float4(vin.PosL, 1.0f), gWorld).xyz;
	vout.NormalW = mul(vin.NormalL, (float3x3)gWorldInvTranspose);

	vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);

	// ʹ�þ���ü���ǰ��Texֵ
	vout.Tex = mul(float4(vin.Tex, 0.0f, 1.0f), gTexTransform).xy;

	return vout;
}