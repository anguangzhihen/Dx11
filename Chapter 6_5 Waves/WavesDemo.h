#pragma once

#ifndef BOXDEMO_H
#define BOXDEMO_H

#include "resource.h"
#include <d3dcompiler.h>
#include <directxmath.h>
#include "d3dApp.h"
#include "Waves.h"


class WavesDemo : public D3DApp
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
	WavesDemo(HINSTANCE hInstance);
	~WavesDemo();

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
	float GetHeight(float x, float z) const;

private:
	ID3D11InputLayout* mVertexLayout;	// 顶点布局
	ID3D11VertexShader* mVertexShader;	// 顶点Shader
	ID3D11PixelShader* mPixelShader;	// 片元Shader

	ID3D11Buffer* mWaveVB;	// 顶点缓冲
	ID3D11Buffer* mWaveIB;	// 索引缓冲
	ID3D11Buffer* mWaveCB;	// 常量缓冲
	ConstantBuffer mCBuffer;	// 常量数据结构

	DirectX::XMFLOAT4X4 mWorld;
	DirectX::XMFLOAT4X4 mView;
	DirectX::XMFLOAT4X4 mProj;

	float mTheta;
	float mPhi;
	float mRadius;

	POINT mLastMousePos;

	int mGridIndexCount;
	Waves mWaves;

	// 申明光栅化阶段状态变量
	ID3D11RasterizerState* mWireframeRS;
};

#endif