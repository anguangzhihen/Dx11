// Color.fx

#include "LightHelper.fx"

// Shader代码
// 顶点着色器的常量缓存
cbuffer cbPerObject : register(b0)
{
	row_major matrix gWorld;	// 默认列主矩阵
	row_major matrix gWorldInvTranspose;	// World逆转置矩阵，用于将法线从模型坐标系中转换到世界坐标系
	row_major matrix gWorldViewProj;	// wvp
};

// 片元着色器的常量缓存
cbuffer cbPerFrame : register(b1)
{
	DirectionalLight gDirLight;	// 直射光
	PointLight gPointLight;	// 点光源
	SpotLight gSpotLight;	// 聚光灯
	Material gMaterial;	// 物体材质
	float3 gEyePosW;	// 当前点到相机向量
};

// 顶点Shader的传入参数
struct VertexIn
{
	float3 PosL : POSITION;
	float3 NormalL : NORMAL;
};

// 顶点Shader返回值，并传入片元Shader
struct VertexOut
{
	float4 PosH : SV_POSITION;
	float3 PosW : POSITION;
	float3 NormalW : NORMAL;
};
