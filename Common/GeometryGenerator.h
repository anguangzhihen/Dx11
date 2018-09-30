#pragma once

#ifndef GEOMETRYGENERATOR
#define GEOMETRYGENERATOR

#include "d3dUtil.h"

class GeometryGenerator
{
public:
	// 一个顶点的数据结构
	struct Vertex
	{
		Vertex() {}
		Vertex(const XMFLOAT3& p, const XMFLOAT3& n, const XMFLOAT3& t, const XMFLOAT2& uv)
			: Position(p), Normal(n), TangentU(t), TexC(uv) {}
		Vertex(float px, float py, float pz,
			float nx, float ny, float nz,
			float tx, float ty, float tz,
			float u, float v)
			: Position(px, py, pz), Normal(nx, ny, nz), TangentU(tx, ty, tz), TexC(u, v) {}

		XMFLOAT3 Position;
		XMFLOAT3 Normal;
		XMFLOAT3 TangentU;
		XMFLOAT2 TexC;
	};

	// Mesh数据
	struct MeshData
	{
		std::vector<Vertex> Vertices;
		std::vector<UINT> Indices;
	};

public:
	// 创建一个方块
	static void CreateBox(float width, float height, float depth, MeshData& meshData);
	// 创建一个面
	static void CreateGrid(float width, float depth, UINT m, UINT n, MeshData& meshData);
	// 创建一个圆柱
	static void CreateCylinder(float bottomRadius, float topRadius, float height, UINT sliceCount, UINT stackCount, MeshData& meshData);
	// 圆柱Top圆形
	static void BuildCylinderTopCap(float bottomRadius, float topRadius, float height, UINT sliceCount, UINT stackCount, MeshData& meshData);
	// 圆柱Bottom圆形
	static void BuildCylinderBottomCap(float bottomRadius, float topRadius, float height, UINT sliceCount, UINT stackCount, MeshData& meshData);
	// 创建球体
	static void CreateSphere(float radius, UINT sliceCount, UINT stackCount, MeshData& meshData);
	// 使用纯三角的方式创建球体
	static void CreateGeosphere(float radius, UINT numSubdivisions, MeshData& meshData);
	// 细分三角
	static void Subdivide(MeshData& meshData);
};

#endif