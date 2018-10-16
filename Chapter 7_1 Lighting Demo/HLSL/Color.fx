// Color.fx

#include "LightHelper.fx"

cbuffer cbPerFrame : register(b0)
{
	DirectionalLight gDirLight;
	PointLight gPointLight;
	SpotLight gSpotLight;
	float3 gEyePosW;
};

cbuffer cbPerObject : register(b1)
{
    row_major matrix gWorld;	// 默认列主矩阵
	row_major matrix gWorldInvTranspose;
	row_major matrix gWorldViewProj;
	Material gMaterial;
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
