#pragma once

#ifndef D3DUTIL_H
#define D3DUTIL_H

#include <wrl/client.h>
#include <assert.h>
#include "dxerr.h"
#include <string>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <DirectXMath.h>

using namespace DirectX;
using namespace std::experimental;

#if defined(DEBUG) | defined(_DEBUG)
	#ifndef HR
	#define HR(x)\
	{\
		HRESULT hr = (x);\
		if (FAILED(hr))\
		{\
			DXTrace(__FILEW__, (DWORD)__LINE__, hr, L#x, true);\
		}\
	}
	#endif
#else
	#ifndef HR
	#define HR(x) (x)
	#endif // !HR
#endif

#define ReleaseCOM(x) { if(x){ x->Release(); x=0; } }

#define Log(x){ std::wostringstream outs; outs << x << L"\n"; OutputDebugString(outs.str().c_str()); }

namespace Colors
{
	XMGLOBALCONST XMVECTORF32 White = { 1.0f, 1.0f, 1.0f, 1.0f };
	XMGLOBALCONST XMVECTORF32 Black = { 0.0f, 0.0f, 0.0f, 1.0f };
	XMGLOBALCONST XMVECTORF32 Red = { 1.0f, 0.0f, 0.0f, 1.0f };
	XMGLOBALCONST XMVECTORF32 Green = { 0.0f, 1.0f, 0.0f, 1.0f };
	XMGLOBALCONST XMVECTORF32 Blue = { 0.0f, 0.0f, 1.0f, 1.0f };
	XMGLOBALCONST XMVECTORF32 Yellow = { 1.0f, 1.0f, 0.0f, 1.0f };
	XMGLOBALCONST XMVECTORF32 Cyan = { 0.0f, 1.0f, 1.0f, 1.0f };
	XMGLOBALCONST XMVECTORF32 Magenta = { 1.0f, 0.0f, 1.0f, 1.0f };

	XMGLOBALCONST XMVECTORF32 Silver = { 0.75f, 0.75f, 0.75f, 1.0f };
	XMGLOBALCONST XMVECTORF32 LightSteelBlue = { 0.69f, 0.77f, 0.87f, 1.0f };
}

class TextHelper
{
public:

	template<typename T>
	static inline std::wstring ToString(const T& s)
	{
		std::wostringstream oss;
		oss << s;

		return oss.str();
	}

	template<typename T>
	static inline T FromString(const std::wstring& s)
	{
		T x;
		std::wistringstream iss(s);
		iss >> x;

		return x;
	}
};

class D3DUtil
{
public :
	static HRESULT CreateShaderFromFile(const WCHAR * objFileNameInOut, const WCHAR * hlslFileName, LPCSTR entryPoint, LPCSTR shaderModel, ID3DBlob ** ppBlobOut)
	{
		HRESULT hr = S_OK;

		// Ѱ���Ƿ����Ѿ�����õĶ�����ɫ��
		if (objFileNameInOut && filesystem::exists(objFileNameInOut))
		{
			HR(D3DReadFileToBlob(objFileNameInOut, ppBlobOut));
		}
		else
		{
			DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
			// ���� D3DCOMPILE_DEBUG ��־���ڻ�ȡ��ɫ��������Ϣ���ñ�־���������������飬
			// ����Ȼ������ɫ�������Ż�����
			dwShaderFlags |= D3DCOMPILE_DEBUG;

			// ��Debug�����½����Ż��Ա������һЩ����������
			dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
			ID3DBlob* errorBlob = nullptr;
			hr = D3DCompileFromFile(hlslFileName, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPoint, shaderModel,
				dwShaderFlags, 0, ppBlobOut, &errorBlob);

			if (errorBlob != nullptr)
			{
				Log((char*)errorBlob->GetBufferPointer());
				ReleaseCOM(errorBlob);
			}
			if (FAILED(hr))
			{
				DXTrace((const WCHAR*)__FILE__, (DWORD)__LINE__, hr, L"CreateShaderFromFile", true);
			}

			// ��ָ��������ļ���������ɫ����������Ϣ���
			if (objFileNameInOut)
			{
				HR(D3DWriteBlobToFile(*ppBlobOut, objFileNameInOut, FALSE));
			}
		}

		return hr;
	}
};

#endif // !D3DUTIL_H



