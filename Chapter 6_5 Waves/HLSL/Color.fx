cbuffer cbPerObject : register(b0)
{
    row_major matrix wvp;	// 默认列主矩阵
};

struct VertexIn
{
	float3 PosL : POSITION;
	float4 Color : COLOR;
};

struct VertexOut
{
	float4 PosH : SV_POSITION;
	float4 Color : COLOR;
};
