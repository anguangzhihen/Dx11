// Color.fx

#include "LightHelper.fx"

cbuffer cbPerObject : register(b0)
{
	row_major matrix gWorld;	// 默认列主矩阵
	row_major matrix gWorldInvTranspose;
	row_major matrix gWorldViewProj;
};

cbuffer cbPerFrame : register(b1)
{
	DirectionalLight gDirLight;
	PointLight gPointLight;
	SpotLight gSpotLight;
	Material gMaterial;
	float3 gEyePosW;
};

struct VertexIn
{
	float3 PosL : POSITION;
	float3 NormalL : NORMAL;
};

struct VertexOut
{
	float4 PosH : SV_POSITION;
	float3 PosW : POSITION;
	float3 NormalW : NORMAL;
};
