#pragma once

#ifndef WAVES_H
#define WAVES_H

#include <Windows.h>
#include <DirectXMath.h>
#include <vector>

#include "MathHelper.h"

using namespace DirectX;

class Waves
{
public:
	Waves();
	~Waves();

	UINT RowCount() const;
	UINT ColumnCount() const;
	UINT VertexCount() const;
	UINT TriangleCount() const;
	float Width()const;
	float Depth()const;

	const XMFLOAT3& operator[](int i) const { return mCurrSolution[i]; }	// 获取当前顶点
	const XMFLOAT3& Normal(int i) const { return mNormals[i]; }	// 获取法线
	const XMFLOAT3& TangentX(int i) const { return mTangentX[i]; }	// 获取当前的切线
	

	void Init(UINT m, UINT n, float dx, float dt, float speed, float damping);
	void Update(float dt);
	void Disturb(UINT i, UINT j, float magnitude);
	void CommonDisturbUpdate(float totalTime, float dt);

	std::vector<UINT> GetIndices();

private:
	UINT mNumRows;
	UINT mNumCols;

	UINT mVertexCount;
	UINT mTriangleCount;

	float mK1;
	float mK2;
	float mK3;

	float mTimeStep;
	float mSpatialStep;

	XMFLOAT3* mPrevSolution;
	XMFLOAT3* mCurrSolution;
	XMFLOAT3* mNormals;
	XMFLOAT3* mTangentX;
};

#endif