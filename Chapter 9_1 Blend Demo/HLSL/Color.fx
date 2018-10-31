// Color.fx

#include "LightHelper.fx"

Texture2D tex : register(t0);
SamplerState sam : register(s0);

cbuffer cbPerObject : register(b0)
{
	row_major matrix gWorld;	// 默认列主矩阵
	row_major matrix gWorldInvTranspose;
	row_major matrix gWorldViewProj;
	row_major matrix gTexTransform;
};

cbuffer cbPerFrame : register(b1)
{
	DirectionalLight gDirLights[3];
	Material gMaterial;
	float3 gEyePosW;
};

struct VertexIn
{
	float3 PosL : POSITION;
	float3 NormalL : NORMAL;
	float2 Tex : TEXCOORD;
};

struct VertexOut
{
	float4 PosH : SV_POSITION;
	float3 PosW : POSITION;
	float3 NormalW : NORMAL;
	float2 Tex : TEXCOORD;
};
