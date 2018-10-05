#include <WindowsX.h>
#include <sstream>
#include <vector>
#include <iostream>
#include <filesystem>

#include "d3dApp.h"
#include "d3dUtil.h"

namespace
{
	// 该全局变量仅用于处理窗口消息
	D3DApp* gd3dApp = 0;
}

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// 在创建窗口的时候，我们就能收到Window传来的消息了，所以在窗口显示前，该函数就要生效
	return gd3dApp->MsgProc(hwnd, msg, wParam, lParam);
}

D3DApp::D3DApp(HINSTANCE hInstance):
	mhAppInst(hInstance),
	mMainWndCaption(L"D3D11 Application"),	// L表明是Unicode 两个字节的字符
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

	// ，效果应该是一样的
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

// 初始化
bool D3DApp::Init()
{
	if (!InitMainWindow())
		return false;
	if (!InitDirect3D())
		return false;
	return true;
}

// 初始化主窗口
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

	// 计算窗口的矩形位置
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

// 初始化Dx
bool D3DApp::InitDirect3D()
{
	UINT createDeviceFlags = 0;

	// DEBUG模式
	#if defined(DEBUG) || defined(_DEBUG)
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
	#endif 

	// 创建设备和设备环境
	D3D_FEATURE_LEVEL featureLevel;
	HRESULT hr = D3D11CreateDevice(
		0,	// 使用默认显卡
		md3dDriverType,	// 一般使用D3D_DRIVER_TYPE_HARDWARE作为参数，不然买来的显卡只能吃灰了
		0,	// 不使用软件驱动
		createDeviceFlags,	// Debug下，我们要求Dx输出一些log信息
		0, 0,	// 默认等级的特性，默认就是11
		D3D11_SDK_VERSION,	// 我们使用Dx11的SDK
		&md3dDevice,
		&featureLevel,
		&md3dImmediateContext);

	if (FAILED(hr))
	{
		// Win10下没有自带图形调试器，所以在使用D3D11_CREATE_DEVICE_DEBUG时会创建失败;
		// 添加以下功能可解决：开始 -> 设置 -> 系统 -> 应用和功能 -> 管理可选功能 -> 添加功能 -> Graphics Tools。
		MessageBox(0, L"D3D11CreateDevice Failed.", 0, 0);
		return false;
	}

	// 显卡不支持dx11
	if (featureLevel != D3D_FEATURE_LEVEL_11_0)
	{
		MessageBox(0, L"Direct3D Reature Level 11 unsupported.", 0, 0);
		return false;
	}

	// 检查设备是否支持多重采样
	HR(md3dDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m4xMsaaQuality));
	assert(m4xMsaaQuality > 0);

	// 缓冲切换链需要完成DXGI_SWAP_CHAIN_DESC结构
	DXGI_SWAP_CHAIN_DESC sd;
	sd.BufferDesc.Width = mClientWidth;	// 缓冲的宽
	sd.BufferDesc.Height = mClientHeight;	// 缓冲的高
	sd.BufferDesc.RefreshRate.Numerator = 60;	// 显示刷新率
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// 使用最经典的32色格式，大部分情况下都使用该格式，可能有些情况下会舍弃Alpha通道
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;	// 显示扫描线模式，似乎是指定图片显示排列是从前往后还是从后往前，这边是未指定
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;	// 缩放模式

	// 使用多重采样MSAA
	if (mEnable4xMsaa)
	{
		sd.SampleDesc.Count = 4;
		sd.SampleDesc.Quality = m4xMsaaQuality - 1;
	}
	else
	{
		// 不使用
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
	}

	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	// 固定，因为我们想要使用后备缓冲
	sd.BufferCount = 1;	// 指定一个后备缓冲
	sd.OutputWindow = mhMainWnd;	// 输出窗口
	sd.Windowed = true;	// 是否窗口化
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;	// 固定，显卡会运行更有效率的方法
	sd.Flags = 0;	// 如果指定为DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH，在切换为全屏时，会选择最适配的桌面，否则在哪个桌面切换到哪个

	// 为了精确获取到SwapChain，必须使用当前设备类查询相关的Factory
	IDXGIDevice* dxgiDevice = 0;
	HR(md3dDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice));

	IDXGIAdapter* dxgiAdapter = 0;
	HR(dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter));

	IDXGIFactory* dxgiFactory = 0;
	HR(dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory));
	HR(dxgiFactory->CreateSwapChain(md3dDevice, &sd, &mSwapChain));	// 根据设备和切换链描述创建切换链

#pragma region 练习4.1至4.5

	// 练习4.1，在CreateSwapChain函数调用后添加
	//HR(dxgiFactory->MakeWindowAssociation(mhMainWnd, DXGI_MWA_NO_WINDOW_CHANGES));

	// 练习4.2
	//UINT i = 0;
	//IDXGIAdapter* adapter = 0;
	//std::vector<IDXGIAdapter*> adapters;
	//while (dxgiFactory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND)
	//{
	//	DXGI_ADAPTER_DESC desc;
	//	if (SUCCEEDED(adapter->GetDesc(&desc)))
	//	{
	//		Log(L"显卡" << i + " " << desc.Description);
	//	}

	//	adapters.push_back(adapter);
	//	i++;
	//}
	//Log(L"显卡数量:" << adapters.size());

	// 练习4.3，CheckInterfaceSupport方法只能用于检测dx10和vista以上系统，如果需要检测则直接创建接口，创建成功则支持（例如 ID3D11Device::CreateBlendState）
	//for (size_t i = 0; i < adapters.size(); i++)
	//{
	//	auto adapter = adapters[i];
	//	LARGE_INTEGER ver;
	//	if (adapter->CheckInterfaceSupport(__uuidof(ID3D10Device), &ver) == S_OK)
	//	{
	//		Log(L"显卡" << i << L"不支持Dx10");
	//	}
	//	else
	//	{
	//		Log(L"显卡" << i << L"支持Dx10");
	//	}

	//	// 使用CreateBlendState函数判断是否支持dx11
	//	ID3D11Device* device;
	//	HRESULT hr = D3D11CreateDevice(
	//		adapter,	// 使用指定的显卡
	//		D3D_DRIVER_TYPE_UNKNOWN,
	//		0,
	//		createDeviceFlags,
	//		0, 0,
	//		D3D11_SDK_VERSION,	// 我们使用Dx11的SDK
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
	//		Log(L"显卡" << i << L"不支持Dx11");
	//	}
	//	else
	//	{
	//		Log(L"显卡" << i << L"支持Dx11");
	//	}
	//	ReleaseCOM(m_BlendState);
	//	ReleaseCOM(device);
	//}

	// 练习4.4
	//auto adapterDef = adapters[0];
	//IDXGIOutput* output;
	//std::vector<IDXGIOutput*> outputs;

	//UINT index = 0;
	//while (adapterDef->EnumOutputs(index, &output) != DXGI_ERROR_NOT_FOUND)
	//{
	//	outputs.push_back(output);
	//	index++;
	//}

	//Log(L"显示器数量:" << outputs.size());

	// 练习4.5
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

	// 练习中清除多余COM
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

// 屏幕尺寸变化时触发
void D3DApp::OnResize()
{
	assert(md3dImmediateContext);
	assert(md3dDevice);
	assert(mSwapChain);

	// 清除之前的显示缓存
	ReleaseCOM(mRenderTargetView);
	ReleaseCOM(mDepthStencilView);
	ReleaseCOM(mDepthStencilBuffer);

	// 根据新的尺寸创建显示缓存
	HR(mSwapChain->ResizeBuffers(1, mClientWidth, mClientHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0));
	ID3D11Texture2D* backBuffer;
	HR(mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer)));	// 创建一个后备缓存
	HR(md3dDevice->CreateRenderTargetView(backBuffer, 0, &mRenderTargetView));	// 创建渲染对象
	ReleaseCOM(backBuffer);

	// 定义深度/模板图，深度图用于确定遮挡关系，模板图一般用于模板测试表明像素点是否能显示
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width = mClientWidth;
	depthStencilDesc.Height = mClientHeight;
	depthStencilDesc.MipLevels = 1;	// 固定不生成mipmap
	depthStencilDesc.ArraySize = 1;	// 固定只生成一张图
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;	// 深度24位，模板8位

	// 重置多重采样
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
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;	// 指定深度/模板图只能由GPU读取写入
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;	// 指定是深度/模板图
	depthStencilDesc.CPUAccessFlags = 0;	// 不给予CPU访问权限
	depthStencilDesc.MiscFlags = 0;

	// 创建深度/模板图
	HR(md3dDevice->CreateTexture2D(&depthStencilDesc, 0, &mDepthStencilBuffer));	
	HR(md3dDevice->CreateDepthStencilView(mDepthStencilBuffer, 0, &mDepthStencilView));

	// 将渲染对象和深度/模板图放到渲染流水线中
	md3dImmediateContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);

	// 设置视窗
	mScreenViewport.TopLeftX = 0;	// 视窗距窗口的X距离
	mScreenViewport.TopLeftY = 0;	// 视窗距窗口的Y距离
	mScreenViewport.Width = static_cast<float>(mClientWidth);	// 宽
	mScreenViewport.Height = static_cast<float>(mClientHeight);	// 高
	mScreenViewport.MinDepth = 0.0f;	// 最小深度
	mScreenViewport.MaxDepth = 1.0f;	// 最大深度

#pragma region 练习4.6 可以运行SkullDemo看到效果

	// 练习4.6
	//mScreenViewport.TopLeftX = 100.0f;
	//mScreenViewport.TopLeftY = 100.0f;
	//mScreenViewport.Width = 500.0f;
	//mScreenViewport.Height = 400.0f;

#pragma endregion

	md3dImmediateContext->RSSetViewports(1, &mScreenViewport);	// 重置视窗
}

// 运行
int D3DApp::Run()
{
	MSG msg = { 0 };
	mTimer.Reset();	// 时间类重置

	// 进入主循环
	while (msg.message != WM_QUIT)
	{
		// 如果收到窗口消息则处理
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// 否则运行游戏逻辑
			mTimer.Tick();

			if (!mAppPaused)
			{
				CalculateFrameStats();	// 更新当前的帧率状态
				UpdateScene(mTimer.DeltaTime());	// 更新场景内容逻辑
				DrawScene();	// 更新场景的渲染
			}
			else
			{
				// 暂停
				Sleep(100);
			}
		}
	}

	// 游戏结束
	return (int)msg.wParam;
}

void D3DApp::CalculateFrameStats()
{
	// 将fps信息显示到窗口bar上
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

// 消息处理
LRESULT D3DApp::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	// 窗口是否活动时的切换
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
		// 保存新的屏幕尺寸
		mClientWidth = LOWORD(lParam);
		mClientHeight = HIWORD(lParam);
		return 0;

	// 玩家开始移动窗口
	case WM_ENTERSIZEMOVE:
		mAppPaused = true;
		mResizing = true;
		mTimer.Stop();
		return 0;

	// 玩家结束移动窗口
	case WM_EXITSIZEMOVE:
		mAppPaused = false;
		mResizing = false;
		mTimer.Start();
		OnResize();
		return 0;

	// 窗口销毁
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	// 当窗口菜单活动时，用户按下一个键，但菜单没有任何处理时会发来这个消息
	case WM_MENUCHAR:
		// 当我们按下alt+enter时，让窗口不会哔
		return MAKELRESULT(0, MNC_CLOSE);

	// 提供给窗口最小尺寸
	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;

	// 处理鼠标事件
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
	
