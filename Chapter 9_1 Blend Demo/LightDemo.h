#pragma once

#ifndef LightDemo_H
#define LightDemo_H

#include <d3dcompiler.h>
#include <directxmath.h>
#include <DDSTextureLoader.h>

#include "resource.h"
#include "d3dApp.h"
#include "LightHelper.h"
#include "Waves.h"

using namespace DirectX;


class LightDemo : public D3DApp
{
public:
	// 顶点Shader的传入参数
	struct Vertex
	{
		XMFLOAT3 Pos;
		XMFLOAT3 Normal;
		XMFLOAT2 Tex;
	};

	// 常量缓存
	struct ConstantBuffer
	{
		XMMATRIX gWorld;
		XMMATRIX gWorldInvTranspose;
		XMMATRIX gWorldViewProj;
		XMMATRIX gTexTransform;
	};

	struct FrameConstantBuffer
	{
		DirectionalLight gDirLights[3];
		Material gMaterial;
		XMFLOAT4 gEyePosW;
		XMFLOAT4 gFogPos;
		XMFLOAT4 gFogColor;
	};


public:
	LightDemo(HINSTANCE hInstance);
	~LightDemo();

	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene();

	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);

private:
	void BuildHillsGeometryBuffers();
	void BuildWavesGeometryBuffers();
	void BuildBoxGeometryBuffers();
	void BuildFX();

	float GetHeight(float x, float z) const;
	XMFLOAT3 GetNormal(float x, float z) const;

private:
	ID3D11InputLayout* mVertexLayout;	// 顶点布局
	ID3D11VertexShader* mVertexShader;	// 顶点Shader
	ID3D11PixelShader* mPixelShader;	// 片元Shader

	ID3D11Buffer* mHillsVB;	// 顶点缓冲
	ID3D11Buffer* mHillsIB;	// 索引缓冲
	ID3D11Buffer* mWavesVB;	// 顶点缓冲
	ID3D11Buffer* mWavesIB;	// 索引缓冲
	ID3D11Buffer* mBoxVB;
	ID3D11Buffer* mBoxIB;

	// 常量缓冲
	ID3D11Buffer* mBoxCB;	
	ID3D11Buffer* mFrameBoxCB;

	// 常量数据结构
	ConstantBuffer mCBuffer;	
	FrameConstantBuffer mFrameCBuffer;

	XMFLOAT4X4 mWavesWorld;
	XMFLOAT4X4 mHillsWorld;
	XMFLOAT4X4 mBoxWorld;
	
	XMFLOAT4X4 mView;
	XMFLOAT4X4 mProj;
	XMFLOAT4X4 mGrassTexTransform;
	XMFLOAT4X4 mWavesTexTransform;
	XMFLOAT4X4 mBoxTexTransform;
	XMFLOAT4 mEyePosW;
	XMFLOAT2 mWavesTexOffset;

	float mTheta;
	float mPhi;
	float mRadius;

	POINT mLastMousePos;

	UINT mGridIndexCount;
	UINT mBoxIndexCount;

	DirectionalLight mDirLights[3];

	Material mHillsMat;
	Material mBoxMat;
	Material mWavesMat;
	Waves mWaves;

	ID3D11ShaderResourceView* mWavesDDS;
	ID3D11ShaderResourceView* mHillsDDS;
	ID3D11ShaderResourceView* mBoxDDS;
	ID3D11SamplerState* mSamplerState;
};

#endif