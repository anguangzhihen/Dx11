#include "GeometryGenerator.h"
#include "MathHelper.h"

void GeometryGenerator::CreateGrid(float width, float depth, UINT m, UINT n, MeshData& meshData) 
{
	UINT vertexCount = m * n;	// 顶点数量
	UINT faceCount = (m - 1)*(n - 1) * 2;	// 三角数量

	float halfWidth = 0.5f*width;
	float halfDepth = 0.5f*depth;

	// 计算一格的宽高
	float dx = width / (n - 1);
	float dz = depth / (m - 1);

	float du = 1.0f / (n - 1);
	float dv = 1.0f / (m - 1);

	// 创建顶点
	meshData.Vertices.resize(vertexCount);
	for (UINT i = 0; i < m; ++i)
	{
		float z = halfDepth - i * dz;
		for (UINT j = 0; j < n; ++j)
		{
			float x = -halfWidth + j * dx;
			meshData.Vertices[i * n + j].Position = XMFLOAT3(x, 0.0f, z);	// 顶点的位置
			meshData.Vertices[i * n + j].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
			meshData.Vertices[i * n + j].TangentU = XMFLOAT3(1.0f, 0.0f, 0.0f);

			// 当前点与00点的xy距离
			meshData.Vertices[i*n + j].TexC.x = j * du;
			meshData.Vertices[i*n + j].TexC.y = i * dv;
		}
	}


	// 创建索引
	meshData.Indices.resize(faceCount * 3);
	UINT k = 0;
	for (UINT i = 0; i < m - 1; ++i)
	{
		for (UINT j = 0; j < n - 1; ++j) 
		{
			// 一个Quad
			meshData.Indices[k] = i * n + j;
			meshData.Indices[k + 1] = i * n + j + 1;
			meshData.Indices[k + 2] = (i + 1) * n + j;

			meshData.Indices[k + 3] = (i + 1) * n + j;
			meshData.Indices[k + 4] = i * n + j + 1;
			meshData.Indices[k + 5] = (i + 1) * n + j + 1;

			k += 6;
		}
	}



}