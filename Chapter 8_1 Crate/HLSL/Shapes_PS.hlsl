#include "Shapes.fx"

//SamplerState samAnisotropic
//{
//	Filter = ANISOTROPIC;
//	MaxAnisotropy = 4;
//
//	AddressU = WRAP;
//	AddressV = WRAP;
//};

float4 PS(VertexOut pin) : SV_Target
{
	pin.NormalW = normalize(pin.NormalW);

	float3 toEyeW = normalize(gEyePosW - pin.PosW);

	float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

	float4 A, D, S;

	// ����ֱ���
	ComputeDirectionalLight(gMaterial, gDirLight, pin.NormalW, toEyeW, A, D, S);
	ambient += A;
	diffuse += D;
	spec += S;

	// ��ͼƬ���в���
	float4 texColor = tex.Sample(sam, pin.Tex);

	// �������еĹ�
	float4 litColor = texColor * (ambient + diffuse) + spec;
	litColor.a = gMaterial.Diffuse.a;

	return litColor;
}