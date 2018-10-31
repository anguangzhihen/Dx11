// Color_PS.hlslƬԪ��ɫ��
#include "Color.fx"

float4 PS(VertexOut pin) : SV_Target
{
	pin.NormalW = normalize(pin.NormalW);

	float distToEye = length(gEyePosW - pin.PosW);
	float3 toEyeW = normalize(gEyePosW - pin.PosW);

	// ��ͼƬ���в���
	float4 texColor = tex.Sample(sam, pin.Tex);

	// clip͸���Ĳ���
	bool gAlphaClip = true;
	if (gAlphaClip)
	{
		clip(texColor.a - 0.1f);
	}

	float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

	float4 A, D, S;

	for (int i = 0; i < 3; i++)
	{
		// ����ֱ���
		ComputeDirectionalLight(gMaterial, gDirLights[i], pin.NormalW, toEyeW, A, D, S);
		ambient += A;
		diffuse += D;
		spec += S;
	}

	// �������еĹ�
	float4 litColor = texColor*(ambient + diffuse) + spec;

	// ��
	bool gFogEnabled = true;
	if (gFogEnabled)
	{
		float fogLerp = saturate((distToEye - gFogPos.x) / gFogPos.y);
		litColor = lerp(litColor, gFogColor, fogLerp);
	}

	litColor.a = gMaterial.Diffuse.a;

	return litColor;
}