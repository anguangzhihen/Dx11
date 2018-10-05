#include <WindowsX.h>
#include <sstream>
#include <vector>
#include <iostream>
#include <filesystem>

#include "d3dApp.h"
#include "d3dUtil.h"

namespace
{
	// ��ȫ�ֱ��������ڴ�������Ϣ
	D3DApp* gd3dApp = 0;
}

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// �ڴ������ڵ�ʱ�����Ǿ����յ�Window��������Ϣ�ˣ������ڴ�����ʾǰ���ú�����Ҫ��Ч
	return gd3dApp->MsgProc(hwnd, msg, wParam, lParam);
}

D3DApp::D3DApp(HINSTANCE hInstance):
	mhAppInst(hInstance),
	mMainWndCaption(L"D3D11 Application"),	// L������Unicode �����ֽڵ��ַ�
	md3dDriverType(D3D_DRIVER_TYPE_HARDWARE),
	mClientWidth(800),
	mClientHeight(600),
	mEnable4xMsaa(false),
	mhMainWnd(0),
	mAppPaused(false),
	mMinimized(false),
	mMaximized(false),
	mResizing(false),
	m4xMsaaQuality(0),
	md3dDevice(0),
	md3dImmediateContext(0),
	mSwapChain(0),
	mDepthStencilBuffer(0),
	mRenderTargetView(0),
	mDepthStencilView(0)
{
	ZeroMemory(&mScreenViewport, sizeof(D3D11_VIEWPORT));
	gd3dApp = this;
}

D3DApp::~D3DApp()
{
	ReleaseCOM(mRenderTargetView);
	ReleaseCOM(mDepthStencilView);
	ReleaseCOM(mSwapChain);
	ReleaseCOM(mDepthStencilBuffer);

	// ��Ч��Ӧ����һ����
	if (md3dImmediateContext)
		md3dImmediateContext->ClearState();

	ReleaseCOM(md3dImmediateContext);
	ReleaseCOM(md3dDevice);
}

HINSTANCE D3DApp::AppInst()const
{
	return mhAppInst;
}

HWND D3DApp::MainWnd()const
{
	return mhMainWnd;
}

float D3DApp::AspectRatio()const
{
	return static_cast<float>(mClientWidth) / mClientHeight;
}

// ��ʼ��
bool D3DApp::Init()
{
	if (!InitMainWindow())
		return false;
	if (!InitDirect3D())
		return false;
	return true;
}

// ��ʼ��������
bool D3DApp::InitMainWindow()
{
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = MainWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = mhAppInst;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = L"D3DWndClassName";

	if (!RegisterClass(&wc))
	{
		MessageBox(0, L"RegisterClass Failed.", 0, 0);
		return false;
	}

	// ���㴰�ڵľ���λ��
	RECT R = { 0, 0, mClientWidth, mClientHeight };
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	int width = R.right - R.left;
	int height = R.bottom - R.top;

	mhMainWnd = CreateWindow(L"D3DWndClassName", mMainWndCaption.c_str(), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, mhAppInst, 0);
	if (!mhMainWnd)
	{
		MessageBox(0, L"CreateWindow Failed", 0, 0);
		return false;
	}

	ShowWindow(mhMainWnd, SW_SHOW);
	UpdateWindow(mhMainWnd);

	return true;
}

// ��ʼ��Dx
bool D3DApp::InitDirect3D()
{
	UINT createDeviceFlags = 0;

	// DEBUGģʽ
	#if defined(DEBUG) || defined(_DEBUG)
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
	#endif 

	// �����豸���豸����
	D3D_FEATURE_LEVEL featureLevel;
	HRESULT hr = D3D11CreateDevice(
		0,	// ʹ��Ĭ���Կ�
		md3dDriverType,	// һ��ʹ��D3D_DRIVER_TYPE_HARDWARE��Ϊ��������Ȼ�������Կ�ֻ�ܳԻ���
		0,	// ��ʹ���������
		createDeviceFlags,	// Debug�£�����Ҫ��Dx���һЩlog��Ϣ
		0, 0,	// Ĭ�ϵȼ������ԣ�Ĭ�Ͼ���11
		D3D11_SDK_VERSION,	// ����ʹ��Dx11��SDK
		&md3dDevice,
		&featureLevel,
		&md3dImmediateContext);

	if (FAILED(hr))
	{
		// Win10��û���Դ�ͼ�ε�������������ʹ��D3D11_CREATE_DEVICE_DEBUGʱ�ᴴ��ʧ��;
		// ������¹��ܿɽ������ʼ -> ���� -> ϵͳ -> Ӧ�ú͹��� -> �����ѡ���� -> ��ӹ��� -> Graphics Tools��
		MessageBox(0, L"D3D11CreateDevice Failed.", 0, 0);
		return false;
	}

	// �Կ���֧��dx11
	if (featureLevel != D3D_FEATURE_LEVEL_11_0)
	{
		MessageBox(0, L"Direct3D Reature Level 11 unsupported.", 0, 0);
		return false;
	}

	// ����豸�Ƿ�֧�ֶ��ز���
	HR(md3dDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m4xMsaaQuality));
	assert(m4xMsaaQuality > 0);

	// �����л�����Ҫ���DXGI_SWAP_CHAIN_DESC�ṹ
	DXGI_SWAP_CHAIN_DESC sd;
	sd.BufferDesc.Width = mClientWidth;	// ����Ŀ�
	sd.BufferDesc.Height = mClientHeight;	// ����ĸ�
	sd.BufferDesc.RefreshRate.Numerator = 60;	// ��ʾˢ����
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// ʹ������32ɫ��ʽ���󲿷�����¶�ʹ�øø�ʽ��������Щ����»�����Alphaͨ��
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;	// ��ʾɨ����ģʽ���ƺ���ָ��ͼƬ��ʾ�����Ǵ�ǰ�����ǴӺ���ǰ�������δָ��
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;	// ����ģʽ

	// ʹ�ö��ز���MSAA
	if (mEnable4xMsaa)
	{
		sd.SampleDesc.Count = 4;
		sd.SampleDesc.Quality = m4xMsaaQuality - 1;
	}
	else
	{
		// ��ʹ��
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
	}

	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	// �̶�����Ϊ������Ҫʹ�ú󱸻���
	sd.BufferCount = 1;	// ָ��һ���󱸻���
	sd.OutputWindow = mhMainWnd;	// �������
	sd.Windowed = true;	// �Ƿ񴰿ڻ�
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;	// �̶����Կ������и���Ч�ʵķ���
	sd.Flags = 0;	// ���ָ��ΪDXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH�����л�Ϊȫ��ʱ����ѡ������������棬�������ĸ������л����ĸ�

	// Ϊ�˾�ȷ��ȡ��SwapChain������ʹ�õ�ǰ�豸���ѯ��ص�Factory
	IDXGIDevice* dxgiDevice = 0;
	HR(md3dDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice));

	IDXGIAdapter* dxgiAdapter = 0;
	HR(dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter));

	IDXGIFactory* dxgiFactory = 0;
	HR(dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory));
	HR(dxgiFactory->CreateSwapChain(md3dDevice, &sd, &mSwapChain));	// �����豸���л������������л���

#pragma region ��ϰ4.1��4.5

	// ��ϰ4.1����CreateSwapChain�������ú����
	//HR(dxgiFactory->MakeWindowAssociation(mhMainWnd, DXGI_MWA_NO_WINDOW_CHANGES));

	// ��ϰ4.2
	//UINT i = 0;
	//IDXGIAdapter* adapter = 0;
	//std::vector<IDXGIAdapter*> adapters;
	//while (dxgiFactory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND)
	//{
	//	DXGI_ADAPTER_DESC desc;
	//	if (SUCCEEDED(adapter->GetDesc(&desc)))
	//	{
	//		Log(L"�Կ�" << i + " " << desc.Description);
	//	}

	//	adapters.push_back(adapter);
	//	i++;
	//}
	//Log(L"�Կ�����:" << adapters.size());

	// ��ϰ4.3��CheckInterfaceSupport����ֻ�����ڼ��dx10��vista����ϵͳ�������Ҫ�����ֱ�Ӵ����ӿڣ������ɹ���֧�֣����� ID3D11Device::CreateBlendState��
	//for (size_t i = 0; i < adapters.size(); i++)
	//{
	//	auto adapter = adapters[i];
	//	LARGE_INTEGER ver;
	//	if (adapter->CheckInterfaceSupport(__uuidof(ID3D10Device), &ver) == S_OK)
	//	{
	//		Log(L"�Կ�" << i << L"��֧��Dx10");
	//	}
	//	else
	//	{
	//		Log(L"�Կ�" << i << L"֧��Dx10");
	//	}

	//	// ʹ��CreateBlendState�����ж��Ƿ�֧��dx11
	//	ID3D11Device* device;
	//	HRESULT hr = D3D11CreateDevice(
	//		adapter,	// ʹ��ָ�����Կ�
	//		D3D_DRIVER_TYPE_UNKNOWN,
	//		0,
	//		createDeviceFlags,
	//		0, 0,
	//		D3D11_SDK_VERSION,	// ����ʹ��Dx11��SDK
	//		&device,0,0);

	//	if (FAILED(hr))
	//	{
	//		MessageBox(0, L"Direct3D Reature Level 11 unsupported.", 0, 0);
	//		continue;
	//	}

	//	ID3D11BlendState* m_BlendState;
	//	D3D11_BLEND_DESC blendDesc = { 0 };
	//	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	//	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA; // Color_Fsrc
	//	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA; // Color_Fdst
	//	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD; // Color_Operation
	//	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE; // Alpha_Fsrc
	//	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO; // Alpha_Fdst
	//	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD; // Alpha_Operation
	//	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	//	if (FAILED(device->CreateBlendState(&blendDesc, &m_BlendState)))
	//	{
	//		Log(L"�Կ�" << i << L"��֧��Dx11");
	//	}
	//	else
	//	{
	//		Log(L"�Կ�" << i << L"֧��Dx11");
	//	}
	//	ReleaseCOM(m_BlendState);
	//	ReleaseCOM(device);
	//}

	// ��ϰ4.4
	//auto adapterDef = adapters[0];
	//IDXGIOutput* output;
	//std::vector<IDXGIOutput*> outputs;

	//UINT index = 0;
	//while (adapterDef->EnumOutputs(index, &output) != DXGI_ERROR_NOT_FOUND)
	//{
	//	outputs.push_back(output);
	//	index++;
	//}

	//Log(L"��ʾ������:" << outputs.size());

	// ��ϰ4.5
	//for (size_t i = 0; i < outputs.size(); i++)
	//{
	//	UINT num = 0;
	//	IDXGIOutput* out = outputs[i];
	//	out->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &num, 0);

	//	DXGI_MODE_DESC* pDescs = new DXGI_MODE_DESC[num];
	//	out->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &num, pDescs);

	//	for (size_t i = 0; i < num; i++)
	//	{
	//		auto desc = pDescs[i];
	//		Log("width:" << desc.Width << " height:" << desc.Height << " Rate:" << desc.RefreshRate.Numerator << "/" << desc.RefreshRate.Denominator);
	//	}
	//}

	// ��ϰ���������COM
	//for (size_t i = 0; i < adapters.size(); i++)
	//{
	//	auto adapter = adapters[i];
	//	ReleaseCOM(adapter);
	//}

#pragma endregion

	ReleaseCOM(dxgiDevice);
	ReleaseCOM(dxgiAdapter);
	ReleaseCOM(dxgiFactory);

	OnResize();
	return true;
}

// ��Ļ�ߴ�仯ʱ����
void D3DApp::OnResize()
{
	assert(md3dImmediateContext);
	assert(md3dDevice);
	assert(mSwapChain);

	// ���֮ǰ����ʾ����
	ReleaseCOM(mRenderTargetView);
	ReleaseCOM(mDepthStencilView);
	ReleaseCOM(mDepthStencilBuffer);

	// �����µĳߴ紴����ʾ����
	HR(mSwapChain->ResizeBuffers(1, mClientWidth, mClientHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0));
	ID3D11Texture2D* backBuffer;
	HR(mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer)));	// ����һ���󱸻���
	HR(md3dDevice->CreateRenderTargetView(backBuffer, 0, &mRenderTargetView));	// ������Ⱦ����
	ReleaseCOM(backBuffer);

	// �������/ģ��ͼ�����ͼ����ȷ���ڵ���ϵ��ģ��ͼһ������ģ����Ա������ص��Ƿ�����ʾ
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width = mClientWidth;
	depthStencilDesc.Height = mClientHeight;
	depthStencilDesc.MipLevels = 1;	// �̶�������mipmap
	depthStencilDesc.ArraySize = 1;	// �̶�ֻ����һ��ͼ
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;	// ���24λ��ģ��8λ

	// ���ö��ز���
	if (mEnable4xMsaa)
	{
		depthStencilDesc.SampleDesc.Count = 4;
		depthStencilDesc.SampleDesc.Quality = m4xMsaaQuality - 1;
	}
	else
	{
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
	}
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;	// ָ�����/ģ��ͼֻ����GPU��ȡд��
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;	// ָ�������/ģ��ͼ
	depthStencilDesc.CPUAccessFlags = 0;	// ������CPU����Ȩ��
	depthStencilDesc.MiscFlags = 0;

	// �������/ģ��ͼ
	HR(md3dDevice->CreateTexture2D(&depthStencilDesc, 0, &mDepthStencilBuffer));	
	HR(md3dDevice->CreateDepthStencilView(mDepthStencilBuffer, 0, &mDepthStencilView));

	// ����Ⱦ��������/ģ��ͼ�ŵ���Ⱦ��ˮ����
	md3dImmediateContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);

	// �����Ӵ�
	mScreenViewport.TopLeftX = 0;	// �Ӵ��ര�ڵ�X����
	mScreenViewport.TopLeftY = 0;	// �Ӵ��ര�ڵ�Y����
	mScreenViewport.Width = static_cast<float>(mClientWidth);	// ��
	mScreenViewport.Height = static_cast<float>(mClientHeight);	// ��
	mScreenViewport.MinDepth = 0.0f;	// ��С���
	mScreenViewport.MaxDepth = 1.0f;	// ������

#pragma region ��ϰ4.6 ��������SkullDemo����Ч��

	// ��ϰ4.6
	//mScreenViewport.TopLeftX = 100.0f;
	//mScreenViewport.TopLeftY = 100.0f;
	//mScreenViewport.Width = 500.0f;
	//mScreenViewport.Height = 400.0f;

#pragma endregion

	md3dImmediateContext->RSSetViewports(1, &mScreenViewport);	// �����Ӵ�
}

// ����
int D3DApp::Run()
{
	MSG msg = { 0 };
	mTimer.Reset();	// ʱ��������

	// ������ѭ��
	while (msg.message != WM_QUIT)
	{
		// ����յ�������Ϣ����
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// ����������Ϸ�߼�
			mTimer.Tick();

			if (!mAppPaused)
			{
				CalculateFrameStats();	// ���µ�ǰ��֡��״̬
				UpdateScene(mTimer.DeltaTime());	// ���³��������߼�
				DrawScene();	// ���³�������Ⱦ
			}
			else
			{
				// ��ͣ
				Sleep(100);
			}
		}
	}

	// ��Ϸ����
	return (int)msg.wParam;
}

void D3DApp::CalculateFrameStats()
{
	// ��fps��Ϣ��ʾ������bar��
	static int frameCnt = 0;
	static float timeElapsed = 0.0f;

	frameCnt++;

	if ((mTimer.TotalTime() - timeElapsed) >= 1.0f)
	{
		float fps = (float)frameCnt;
		float mspf = 1000.0f / fps;

		std::wostringstream outs;
		outs.precision(6);
		outs << mMainWndCaption << L"   "
			<< L"FPS:" << fps << L"   "
			<< L"Frame Time:" << mspf << L"(ms)";
		SetWindowText(mhMainWnd, outs.str().c_str());
		frameCnt = 0;
		timeElapsed += 1.0f;
	}
}

// ��Ϣ����
LRESULT D3DApp::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	// �����Ƿ�ʱ���л�
	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			mAppPaused = true;
			mTimer.Stop();
		}
		else
		{
			mAppPaused = false;
			mTimer.Start();
		}
		return 0;
	case WM_SIZE:
		// �����µ���Ļ�ߴ�
		mClientWidth = LOWORD(lParam);
		mClientHeight = HIWORD(lParam);
		return 0;

	// ��ҿ�ʼ�ƶ�����
	case WM_ENTERSIZEMOVE:
		mAppPaused = true;
		mResizing = true;
		mTimer.Stop();
		return 0;

	// ��ҽ����ƶ�����
	case WM_EXITSIZEMOVE:
		mAppPaused = false;
		mResizing = false;
		mTimer.Start();
		OnResize();
		return 0;

	// ��������
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	// �����ڲ˵��ʱ���û�����һ���������˵�û���κδ���ʱ�ᷢ�������Ϣ
	case WM_MENUCHAR:
		// �����ǰ���alt+enterʱ���ô��ڲ�����
		return MAKELRESULT(0, MNC_CLOSE);

	// �ṩ��������С�ߴ�
	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;

	// ��������¼�
	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
		OnMouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
		OnMouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_MOUSEMOVE:
		OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	}
	
	return DefWindowProc(hwnd, msg, wParam, lParam);
}
	
