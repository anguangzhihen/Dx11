#pragma once


#ifndef LIGHTHELPER_H
#define LIGHTHELPER_H

#include <Windows.h>
#include <DirectXMath.h>

using namespace DirectX;

// ֱ���
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
	float Pad;	// ����һλ����֤128bit����
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