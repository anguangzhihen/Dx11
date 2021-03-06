// Init Direct3D.cpp : 定义应用程序的入口点。
//

#include "Init Direct3D.h"
#include "d3dApp.h"
#include "d3dUtil.h"

// 最简单的Dx框架类实现
class InitDirect3DApp : public D3DApp
{
public:
	InitDirect3DApp(HINSTANCE hInstance);
	~InitDirect3DApp();

	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene();
};

// 程序入口
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	// 开启实时内存检测
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	// 创建游戏运行对象
	InitDirect3DApp theApp(hInstance);
	if (!theApp.Init())
		return 0;
	return theApp.Run();
}

InitDirect3DApp::InitDirect3DApp(HINSTANCE hInstance) : D3DApp(hInstance)
{
}

InitDirect3DApp::~InitDirect3DApp()
{
}

// 初始化
bool InitDirect3DApp::Init()
{
	if (!D3DApp::Init())
		return false;
	return true;
}

// 在玩家改变屏幕分辨率时调用
void InitDirect3DApp::OnResize()
{
	D3DApp::OnResize();
}

// 每帧都会调用一次，用于更新当前的逻辑，dt为上一帧到当前帧的时间，类似于Unity中的Update
void InitDirect3DApp::UpdateScene(float dt)
{
}

// 渲染当前场景，类似Unity生命周期中的Scene rendering阶段（OnWillRenderObject、OnRenderImage等方法）
void InitDirect3DApp::DrawScene()
{
	assert(md3dImmediateContext);
	assert(mSwapChain);

	// 清空当前的渲染对象，并所有像素点默认填上蓝色
	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&(Colors::Blue)));
	// 清除深度和模版buffer，深度默认为最远1（范围0到1），深度和模版是以像素点为最小单位的，所以如果一个800*600的画面的话，深度和模版值也会有800*600个
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// 切换显示和后备缓冲
	mSwapChain->Present(0, 0);
}




