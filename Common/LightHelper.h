#pragma once


#ifndef LIGHTHELPER_H
#define LIGHTHELPER_H

#include <Windows.h>
#include <DirectXMath.h>

using namespace DirectX;

// 直射光
struct DirectionalLight 
{
	DirectionalLight() 
	{
		ZeroMemory(this, sizeof(this));
	}

	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular;
	XMFLOAT3 Direction;
	float Pad;	// 填补最后一位，保证128bit对齐
};

struct PointLight
{
	PointLight()
	{
		ZeroMemory(this, sizeof(this));
	}

	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular;

	XMFLOAT3 Position;
	float Range;

	XMFLOAT3 Att;
	float Pad;
};

struct SpotLight
{
	SpotLight()
	{
		ZeroMemory(this, sizeof(this));
	}

	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular;
	XMFLOAT3 Position;
	float Range;
	XMFLOAT3 Direction;
	float Spot;
	XMFLOAT3 Att;
	float Pad;
};

struct Material
{
	Material() {
		ZeroMemory(this, sizeof(this));
	}

	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular;
	XMFLOAT4 Reflect;

};

#endif