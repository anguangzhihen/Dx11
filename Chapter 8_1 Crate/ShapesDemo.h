#pragma once

#ifndef SHAPESDEMO_H
#define SHAPESDEMO_H

#include <vector>

#include <d3dcompiler.h>
#include <directxmath.h>
#include <DDSTextureLoader.h>
#include <WICTextureLoader.h>

#include "resource.h"
#include "d3dApp.h"
#include "LightHelper.h"

using namespace DirectX;

class ShapesDemo : public D3DApp
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
		DirectionalLight gDirLight;
		Material gMaterial;
		XMFLOAT4 gEyePosW;
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
		
	// 常量缓冲
	ID3D11Buffer* mBoxCB;	
	ID3D11Buffer* mFrameBoxCB;

	// 常量数据结构
	ConstantBuffer mCBuffer;
	FrameConstantBuffer mFrameCBuffer;

	float mTheta;
	float mPhi;
	float mRadius;

	POINT mLastMousePos;

	UINT totalVertexCount;
	UINT totalIndexCount;

	int mBoxVertexOffset;
	int mBoxIndexCount;
	int mBoxIndexOffset;

	XMFLOAT4X4 mBoxWorld;
	XMFLOAT4X4 mView;
	XMFLOAT4X4 mProj;
	XMFLOAT4X4 mTexTransform;
	XMFLOAT4 mEyePosW;

	DirectionalLight mDirLight;
	Material mMat;

	ID3D11ShaderResourceView* mWoodCrateDDS;
	ID3D11ShaderResourceView* mSecondDDS;	// 练习8.4使用
	std::vector<ID3D11ShaderResourceView*> mFires;	// 练习8.5
	ID3D11SamplerState* mSamplerState;
};

#endif