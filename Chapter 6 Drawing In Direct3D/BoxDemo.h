#pragma once

#ifndef BOXDEMO_H
#define BOXDEMO_H

#include "resource.h"
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxpackedvector.h>
#include "d3dApp.h"

using namespace DirectX;
using namespace DirectX::PackedVector;

class BoxApp : public D3DApp
{
public:
	// 顶点Shader的传入参数
	struct Vertex
	{
		DirectX::XMFLOAT3 Pos;
		DirectX::XMFLOAT4 Color;
	};

	struct VertexPos
	{
		DirectX::XMFLOAT3 Pos;
	};

	struct VertexColor
	{
		DirectX::XMFLOAT4 Color;
	};

	struct Vertex32bitColor
	{
		DirectX::XMFLOAT3 Pos;
		XMCOLOR Color;
	};

	// 常量缓存
	struct ConstantBuffer
	{
		DirectX::XMMATRIX wvp;
		float gTime;
	};

public:
	BoxApp(HINSTANCE hInstance);
	~BoxApp();

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

	DirectX::XMFLOAT4X4 mWorld;
	DirectX::XMFLOAT4X4 mView;
	DirectX::XMFLOAT4X4 mProj;

	float mTheta;
	float mPhi;
	float mRadius;

	POINT mLastMousePos;
	ID3D11RasterizerState* mWireframeRS;
};

#endif