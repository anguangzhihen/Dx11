#include "GeometryGenerator.h"
#include "MathHelper.h"

void GeometryGenerator::CreateGrid(float width, float depth, UINT m, UINT n, MeshData& meshData) 
{
	UINT vertexCount = m * n;	// ��������
	UINT faceCount = (m - 1)*(n - 1) * 2;	// ��������

	float halfWidth = 0.5f*width;
	float halfDepth = 0.5f*depth;

	// ����һ��Ŀ��
	float dx = width / (n - 1);
	float dz = depth / (m - 1);

	float du = 1.0f / (n - 1);
	float dv = 1.0f / (m - 1);

	// ��������
	meshData.Vertices.resize(vertexCount);
	for (UINT i = 0; i < m; ++i)
	{
		float z = halfDepth - i * dz;
		for (UINT j = 0; j < n; ++j)
		{
			float x = -halfWidth + j * dx;
			meshData.Vertices[i * n + j].Position = XMFLOAT3(x, 0.0f, z);	// �����λ��
			meshData.Vertices[i * n + j].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
			meshData.Vertices[i * n + j].TangentU = XMFLOAT3(1.0f, 0.0f, 0.0f);

			// ��ǰ����00���xy����
			meshData.Vertices[i*n + j].TexC.x = j * du;
			meshData.Vertices[i*n + j].TexC.y = i * dv;
		}
	}


	// ��������
	meshData.Indices.resize(faceCount * 3);
	UINT k = 0;
	for (UINT i = 0; i < m - 1; ++i)
	{
		for (UINT j = 0; j < n - 1; ++j) 
		{
			// һ��Quad
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