#include "BoxDemo.h"
#include "d3dUtil.h"
#include "MathHelper.h"

#include <filesystem>

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

	BoxApp theApp(hInstance);
	if (!theApp.Init())
		return 0;
	return theApp.Run();
}

BoxApp::BoxApp(HINSTANCE hInstance) : D3DApp(hInstance), 
	mVertexShader(0), mPixelShader(0), mVertexLayout(0),
	mBoxVB(0), mBoxIB(0), mBoxCB(0), 
	mTheta(1.5f * MathHelper::Pi), mPhi(0.25f*MathHelper::Pi), mRadius(5.0f)
{
	mMainWndCaption = L"Box Demo";

	mLastMousePos.x = 0;
	mLastMousePos.y = 0;

	XMMATRIX I = XMMatrixIdentity();

	// 默认为单位矩阵
	// 行主矩阵，单精度浮点值
	DirectX::XMStoreFloat4x4(&mWorld, I);
	DirectX::XMStoreFloat4x4(&mView, I);
	DirectX::XMStoreFloat4x4(&mProj, I);
}

BoxApp::~BoxApp()
{
	ReleaseCOM(mBoxVB);
	ReleaseCOM(mBoxIB);
	ReleaseCOM(mBoxCB);
	ReleaseCOM(mVertexLayout);
	ReleaseCOM(mVertexShader);
	ReleaseCOM(mPixelShader);
}

bool BoxApp::Init()
{
	if (!D3DApp::Init())
		return false;

	BuildGeometryBuffers();
	BuildFX();

#pragma region 练习6.8

	//D3D11_RASTERIZER_DESC wireframeDesc;
	//ZeroMemory(&wireframeDesc, sizeof(D3D11_RASTERIZER_DESC));
	//wireframeDesc.FillMode = D3D11_FILL_WIREFRAME;
	//wireframeDesc.CullMode = D3D11_CULL_BACK;
	//wireframeDesc.FrontCounterClockwise = false;
	//wireframeDesc.DepthClipEnable = true;

	//HR(md3dDevice->CreateRasterizerState(&wireframeDesc, &mWireframeRS));

#pragma endregion

	return true;
}

void BoxApp::OnResize()
{
	D3DApp::OnResize();

	// 当窗口尺寸变化后，重置投影矩阵
	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f * MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
	DirectX::XMStoreFloat4x4(&mProj, P);
}

// 创建顶点和索引信息，并设置GPU的缓存
void BoxApp::BuildGeometryBuffers()
{
	// 方块坍缩前的信息
	//Vertex vertices[] =
	//{
	//	{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4((const float*)&Colors::White) },
	//	{ XMFLOAT3(-1.0f, +1.0f, -1.0f), XMFLOAT4((const float*)&Colors::Black) },
	//	{ XMFLOAT3(+1.0f, +1.0f, -1.0f), XMFLOAT4((const float*)&Colors::Red) },
	//	{ XMFLOAT3(+1.0f, -1.0f, -1.0f), XMFLOAT4((const float*)&Colors::Green) },
	//	{ XMFLOAT3(-1.0f, -1.0f, +1.0f), XMFLOAT4((const float*)&Colors::Blue) },
	//	{ XMFLOAT3(-1.0f, +1.0f, +1.0f), XMFLOAT4((const float*)&Colors::Yellow) },
	//	{ XMFLOAT3(+1.0f, +1.0f, +1.0f), XMFLOAT4((const float*)&Colors::Cyan) },
	//	{ XMFLOAT3(+1.0f, -1.0f, +1.0f), XMFLOAT4((const float*)&Colors::Magenta) },
	//};
	//UINT indices[] = {
	//	0,1,2,  0,2,3,
	//	4,6,5,  4,7,6,
	//	4,5,1,  4,1,0,
	//	3,2,6,  3,6,7,
	//	1,5,6,  1,6,2,
	//	4,0,3,  4,3,7,
	//};

	// 顶点
	std::vector<Vertex> vertices = 
	{
		{ XMFLOAT3(-1.0f, 0.0f, -1.0f), XMFLOAT4((const float*)&Colors::Red) },
		{ XMFLOAT3(-1.0f, 0.0f, +1.0f), XMFLOAT4((const float*)&Colors::Green) },
		{ XMFLOAT3(+1.0f, 0.0f, +1.0f), XMFLOAT4((const float*)&Colors::Blue) },
		{ XMFLOAT3(+1.0f, 0.0f, -1.0f), XMFLOAT4((const float*)&Colors::Yellow) },
		{ XMFLOAT3(0.0f, 1.41f, 0.0f), XMFLOAT4((const float*)&Colors::Black) },
	};

#pragma region 练习6.4，就是我们探索的方块，只是颜色有点不一样

	//std::vector<Vertex> vertices =
	//{
	//	{ XMFLOAT3(-1.0f, 0.0f, -1.0f), XMFLOAT4((const float*)&Colors::Green) },
	//	{ XMFLOAT3(-1.0f, 0.0f, +1.0f), XMFLOAT4((const float*)&Colors::Green) },
	//	{ XMFLOAT3(+1.0f, 0.0f, +1.0f), XMFLOAT4((const float*)&Colors::Green) },
	//	{ XMFLOAT3(+1.0f, 0.0f, -1.0f), XMFLOAT4((const float*)&Colors::Green) },
	//	{ XMFLOAT3(0.0f, 1.41f, 0.0f), XMFLOAT4((const float*)&Colors::Red) },
	//};

#pragma endregion

	// 顶点缓存的描述
	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D11_USAGE_DEFAULT;
	vbd.ByteWidth = sizeof(Vertex) * vertices.size();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;

	// 将顶点设置到描述中
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = &vertices[0];

	// 创建缓存
	HR(md3dDevice->CreateBuffer(&vbd, &InitData, &mBoxVB));

	// 绑定到渲染管线上
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	md3dImmediateContext->IASetVertexBuffers(0, 1, &mBoxVB, &stride, &offset);

#pragma region 练习6.2 运行前将上面的顶点缓存创建给注释掉

	//std::vector<VertexPos> verticesPos =
	//{
	//	{ XMFLOAT3(-1.0f, 0.0f, -1.0f) },
	//	{ XMFLOAT3(-1.0f, 0.0f, +1.0f) },
	//	{ XMFLOAT3(+1.0f, 0.0f, +1.0f) },
	//	{ XMFLOAT3(+1.0f, 0.0f, -1.0f) },
	//	{ XMFLOAT3(0.0f, 1.41f, 0.0f) },
	//};

	//// 顶点缓存 Position
	//D3D11_BUFFER_DESC vbdPos;
	//ZeroMemory(&vbdPos, sizeof(vbdPos));
	//vbdPos.Usage = D3D11_USAGE_DEFAULT;
	//vbdPos.ByteWidth = sizeof(VertexPos) * verticesPos.size();
	//vbdPos.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	//vbdPos.CPUAccessFlags = 0;

	//D3D11_SUBRESOURCE_DATA InitData;
	//ZeroMemory(&InitData, sizeof(InitData));
	//InitData.pSysMem = &verticesPos[0];
	//HR(md3dDevice->CreateBuffer(&vbdPos, &InitData, &mBoxVB));

	//UINT stride = sizeof(VertexPos);
	//UINT offset = 0;
	//md3dImmediateContext->IASetVertexBuffers(0, 2, &mBoxVB, &stride, &offset);

	//std::vector<VertexColor> verticesColor = 
	//{
	//	{ XMFLOAT4((const float*)&Colors::Red) },
	//	{ XMFLOAT4((const float*)&Colors::Green) },
	//	{ XMFLOAT4((const float*)&Colors::Blue) },
	//	{ XMFLOAT4((const float*)&Colors::Yellow) },
	//	{ XMFLOAT4((const float*)&Colors::Black) },
	//};

	//// 顶点缓存 Color
	//D3D11_BUFFER_DESC vbdColor;
	//ZeroMemory(&vbdColor, sizeof(vbdColor));
	//vbdColor.Usage = D3D11_USAGE_DEFAULT;
	//vbdColor.ByteWidth = sizeof(VertexColor) * verticesColor.size();
	//vbdColor.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	//vbdColor.CPUAccessFlags = 0;

	//ZeroMemory(&InitData, sizeof(InitData));
	//InitData.pSysMem = &verticesColor[0];
	//HR(md3dDevice->CreateBuffer(&vbdColor, &InitData, &mBoxVB));

	//stride = sizeof(VertexColor);
	//offset = 0;
	//md3dImmediateContext->IASetVertexBuffers(1, 2, &mBoxVB, &stride, &offset);

#pragma endregion

	// 创建索引
	std::vector<UINT> indices =
	{
		// 底面
		0, 2, 1,
		0, 3, 2,	
		// 4个三角面
		0, 1, 4,	
		1, 2, 4,
		2, 3, 4,
		3, 0, 4,
	};

	// 索引缓存的描述
	D3D11_BUFFER_DESC ibd;
	ZeroMemory(&ibd, sizeof(ibd));
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.ByteWidth = sizeof(UINT) * indices.size();
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;

	// 将索引设置到描述中
	InitData.pSysMem = &indices[0];
	HR(md3dDevice->CreateBuffer(&ibd, &InitData, &mBoxIB));

	// 绑定到渲染管线上
	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	// 设定图元类型
	md3dImmediateContext->IASetIndexBuffer(mBoxIB, DXGI_FORMAT_R32_UINT, 0);

	
	// 设置常量缓存区
	D3D11_BUFFER_DESC cbd;
	ZeroMemory(&cbd, sizeof(cbd));
	cbd.Usage = D3D11_USAGE_DEFAULT;
	cbd.ByteWidth = sizeof(ConstantBuffer);
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.CPUAccessFlags = 0;
	HR(md3dDevice->CreateBuffer(&cbd, nullptr, &mBoxCB));
	md3dImmediateContext->VSSetConstantBuffers(0, 1, &mBoxCB);
}

// 设置着色器
void BoxApp::BuildFX()
{
	ID3DBlob* blob;

	// 创建顶点着色器
	HR(D3DUtil::CreateShaderFromFile(L"HLSL\\Color_VS.vso", L"HLSL\\Color_VS.hlsl", "VS", "vs_5_0", &blob));
	HR(md3dDevice->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &mVertexShader));

	// 创建顶点布局
	D3D11_INPUT_ELEMENT_DESC vertexLayout[2] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

#pragma region 练习6.1

	// 练习6.1
	//D3D11_INPUT_ELEMENT_DESC vertexLayout[6] = {
	//	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//	{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 36, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//	{ "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 52, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	//};

#pragma endregion

#pragma region 练习6.2 运行前把上面创建顶点布局给注释掉

	//D3D11_INPUT_ELEMENT_DESC vertexLayout[2] = {
	//	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	//};

#pragma endregion

	HR(md3dDevice->CreateInputLayout(vertexLayout, ARRAYSIZE(vertexLayout), blob->GetBufferPointer(), blob->GetBufferSize(), &mVertexLayout));
	md3dImmediateContext->IASetInputLayout(mVertexLayout);// 设定输入布局
	ReleaseCOM(blob);

	// 创建像素着色器
	HR(D3DUtil::CreateShaderFromFile(L"HLSL\\Color_PS.pso", L"HLSL\\Color_PS.hlsl", "PS", "ps_5_0", &blob));
	HR(md3dDevice->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &mPixelShader));
	ReleaseCOM(blob);

	md3dImmediateContext->VSSetShader(mVertexShader, nullptr, 0);
	md3dImmediateContext->PSSetShader(mPixelShader, nullptr, 0);
}

void BoxApp::UpdateScene(float dt)
{
	float x = mRadius * sinf(mPhi) * cosf(mTheta);
	float z = mRadius * sinf(mPhi) * sinf(mTheta);
	float y = mRadius * cosf(mPhi);

	XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX V = XMMatrixLookAtLH(pos, target, up);
	DirectX::XMStoreFloat4x4(&mView, V);

	// Unity Matrix4x4 列优先填充  Unity Shader float4x4 行优先填充
	DirectX::XMMATRIX world = DirectX::XMLoadFloat4x4(&mWorld);	// 主行矩阵
	DirectX::XMMATRIX view = DirectX::XMLoadFloat4x4(&mView);
	DirectX::XMMATRIX proj = DirectX::XMLoadFloat4x4(&mProj);

	mCBuffer.wvp = world * view * proj;
	mCBuffer.gTime = mTimer.TotalTime();

	// 更新WVP矩阵数据
	md3dImmediateContext->UpdateSubresource(mBoxCB, 0, nullptr, &mCBuffer, 0, 0);
}

void BoxApp::DrawScene()
{
	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::LightSteelBlue));
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

#pragma region 练习6.8
	//md3dImmediateContext->RSSetState(mWireframeRS);
#pragma endregion

	// 绘制Box的36的顶点（12个三角）
	//md3dImmediateContext->DrawIndexed(36, 0, 0);
	md3dImmediateContext->DrawIndexed(18, 0, 0);

	HR(mSwapChain->Present(0, 0));
}

#pragma region Input

void BoxApp::OnMouseDown(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;
	SetCapture(mhMainWnd);	// 捕获鼠标
}

void BoxApp::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void BoxApp::OnMouseMove(WPARAM btnState, int x, int y)
{
	if ((btnState & MK_LBUTTON) != 0)
	{
		// 计算鼠标移动的距离
		float dx = XMConvertToRadians(0.25f*static_cast<float>(x - mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f*static_cast<float>(y - mLastMousePos.y));

		// 更新视角信息
		mTheta += dx;
		mPhi += dy;

		// 裁剪mPhi角度
		mPhi = MathHelper::Clamp(mPhi, 0.1f, MathHelper::Pi - 0.1f);
	}
	else if ((btnState & MK_RBUTTON) != 0)
	{
		// 设定每个像素点为0.005的距离
		float dx = 0.005f*static_cast<float>(x - mLastMousePos.x);
		float dy = 0.005f*static_cast<float>(y - mLastMousePos.y);

		// 更新相机与物体的距离
		mRadius += dx - dy;

		// 裁剪距离
		mRadius = MathHelper::Clamp(mRadius, 3.0f, 15.0f);
	}

	mLastMousePos.x = x;
	mLastMousePos.y = y;
}

#pragma endregion

