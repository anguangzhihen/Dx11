#pragma once

#ifndef SHAPESDEMO_H
#define SHAPESDEMO_H

#include "resource.h"
#include <d3dcompiler.h>
#include <directxmath.h>
#include "d3dApp.h"


class ShapesDemo : public D3DApp
{
public:
	// 顶点Shader的传入参数
	struct Vertex
	{
		DirectX::XMFLOAT3 Pos;
		DirectX::XMFLOAT4 Color;
	};

	// 常量缓存
	struct ConstantBuffer
	{
		DirectX::XMMATRIX wvp;
	};

public:
	ShapesDemo(HINSTANCE hInstance);
	~ShapesDemo();

	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene();

	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);

private:
	void BuildGeometryBuffers();
	void BuildFX();

private:
	ID3D11InputLayout* mVertexLayout;	// 顶点布局
	ID3D11VertexShader* mVertexShader;	// 顶点Shader
	ID3D11PixelShader* mPixelShader;	// 片元Shader

	ID3D11Buffer* mBoxVB;	// 顶点缓冲
	ID3D11Buffer* mBoxIB;	// 索引缓冲
	ID3D11Buffer* mBoxCB;	// 常量缓冲
	ConstantBuffer mCBuffer;	// 常量数据结构



	float mTheta;
	float mPhi;
	float mRadius;

	POINT mLastMousePos;

	UINT totalVertexCount;
	UINT totalIndexCount;

	int mBoxVertexOffset;
	int mBoxIndexCount;
	int mBoxIndexOffset;

	int mGridVertexOffset;
	int mGridIndexCount;
	int mGridIndexOffset;

	int mSphereVertexOffset;
	int mSphereIndexCount;
	int mSphereIndexOffset;

	int mCylinderVertexOffset;
	int mCylinderIndexCount;
	int mCylinderIndexOffset;

	DirectX::XMFLOAT4X4 mSphereWorld[10];
	DirectX::XMFLOAT4X4 mCylWorld[10];
	DirectX::XMFLOAT4X4 mBoxWorld;
	DirectX::XMFLOAT4X4 mGridWorld;
	DirectX::XMFLOAT4X4 mCenterSphere;

	DirectX::XMFLOAT4X4 mView;
	DirectX::XMFLOAT4X4 mProj;

	ID3D11RasterizerState* mWireframeRS;
};

#endif