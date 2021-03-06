#pragma once

#ifndef LightDemo_H
#define LightDemo_H

#include "resource.h"
#include <d3dcompiler.h>
#include <directxmath.h>
#include "d3dApp.h"
#include "LightHelper.h"
#include "Waves.h"

using namespace DirectX;


class LightDemo : public D3DApp
{
public:
	// C++代码
	// 顶点Shader的传入参数
	struct Vertex
	{
		XMFLOAT3 Pos;	// 位置信息
		XMFLOAT3 Normal;	// 法线
	};

	// 顶点着色器的常量缓存
	struct ConstantBuffer
	{
		XMMATRIX gWorld;
		XMMATRIX gWorldInvTranspose;	// World逆转置矩阵，用于将法线从模型坐标系中转换到世界坐标系
		XMMATRIX gWorldViewProj;	 // wvp
	};

	// 片元着色器的常量缓存
	struct FrameConstantBuffer
	{
		DirectionalLight gDirLight;	// 直射光
		PointLight gPointLight;	// 点光源
		SpotLight gSpotLight;	// 聚光灯
		Material gMaterial;	// 物体材质
		XMFLOAT4 gEyePosW;	// 当前点到相机向量
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
	void BuildWavesGemetryBuffers();
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

	// 常量缓冲
	ID3D11Buffer* mBoxCB;
	ID3D11Buffer* mFrameBoxCB;

	// 常量数据结构
	ConstantBuffer mCBuffer;
	FrameConstantBuffer mFrameCBuffer;

	DirectX::XMFLOAT4X4 mWavesWorld;
	DirectX::XMFLOAT4X4 mHillsWorld;
	DirectX::XMFLOAT4X4 mView;
	DirectX::XMFLOAT4X4 mProj;
	XMFLOAT4 mEyePosW;

	float mTheta;
	float mPhi;
	float mRadius;

	POINT mLastMousePos;

	UINT mGridIndexCount;

	DirectionalLight mDirLight;
	PointLight mPointLight;
	SpotLight mSpotLight;

	Material mHillsMat;
	Material mWavesMat;

	Waves mWaves;
};

#endif