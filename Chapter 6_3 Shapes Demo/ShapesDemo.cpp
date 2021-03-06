#include "ShapesDemo.h"
#include "d3dUtil.h"
#include "MathHelper.h"
#include "GeometryGenerator.h"

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

	ShapesDemo theApp(hInstance);
	if (!theApp.Init())
		return 0;
	return theApp.Run();
}

ShapesDemo::ShapesDemo(HINSTANCE hInstance) : D3DApp(hInstance),
mVertexShader(0), mPixelShader(0), mVertexLayout(0), mWireframeRS(0),
mBoxVB(0), mBoxIB(0), mBoxCB(0),
mTheta(1.5f * MathHelper::Pi), mPhi(0.25f*MathHelper::Pi), mRadius(5.0f)
{
	mMainWndCaption = L"Box Demo";

	mLastMousePos.x = 0;
	mLastMousePos.y = 0;

	XMMATRIX I = XMMatrixIdentity();

	// 默认为单位矩阵
	// 行主矩阵，单精度浮点值
	DirectX::XMStoreFloat4x4(&mView, I);
	DirectX::XMStoreFloat4x4(&mProj, I);

	// 初始化各个模型的位置
	DirectX::XMStoreFloat4x4(&mGridWorld, I);

	XMMATRIX boxScale = XMMatrixScaling(2.0f, 1.0f, 2.0f);
	XMMATRIX boxOffset = XMMatrixTranslation(0.0f, 0.5f, 0.0f);
	XMStoreFloat4x4(&mBoxWorld, XMMatrixMultiply(boxScale, boxOffset));

	XMMATRIX centerSphereScale = XMMatrixScaling(2.0f, 2.0f, 2.0f);
	XMMATRIX centerSphereOffset = XMMatrixTranslation(0.0f, 2.0f, 0.0f);
	XMStoreFloat4x4(&mCenterSphere, XMMatrixMultiply(centerSphereScale, centerSphereOffset));

	for (size_t i = 0; i < 5; i++)
	{
		XMStoreFloat4x4(&mCylWorld[i * 2 + 0], XMMatrixTranslation(-5.0f, 1.5f, -10.0f + i * 5.0f));
		XMStoreFloat4x4(&mCylWorld[i * 2 + 1], XMMatrixTranslation(+5.0f, 1.5f, -10.0f + i * 5.0f));

		XMStoreFloat4x4(&mSphereWorld[i * 2 + 0], XMMatrixTranslation(-5.0f, 3.5f, -10.0f + i * 5.0f));
		XMStoreFloat4x4(&mSphereWorld[i * 2 + 1], XMMatrixTranslation(+5.0f, 3.5f, -10.0f + i * 5.0f));
	}

}

ShapesDemo::~ShapesDemo()
{
	ReleaseCOM(mBoxVB);
	ReleaseCOM(mBoxIB);
	ReleaseCOM(mBoxCB);
	ReleaseCOM(mVertexLayout);
	ReleaseCOM(mVertexShader);
	ReleaseCOM(mPixelShader);
	ReleaseCOM(mWireframeRS);
}

bool ShapesDemo::Init()
{
	if (!D3DApp::Init())
		return false;

	BuildGeometryBuffers();
	BuildFX();

	D3D11_RASTERIZER_DESC wireframeDesc;
	ZeroMemory(&wireframeDesc, sizeof(D3D11_RASTERIZER_DESC));
	wireframeDesc.FillMode = D3D11_FILL_WIREFRAME;
	wireframeDesc.CullMode = D3D11_CULL_BACK;
	wireframeDesc.FrontCounterClockwise = false;
	wireframeDesc.DepthClipEnable = true;

#pragma region 练习6.13

	//wireframeDesc.ScissorEnable = true;

#pragma endregion

	HR(md3dDevice->CreateRasterizerState(&wireframeDesc, &mWireframeRS));

	return true;
}

void ShapesDemo::OnResize()
{
	D3DApp::OnResize();

	// 当窗口尺寸变化后，重置投影矩阵
	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f * MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
	DirectX::XMStoreFloat4x4(&mProj, P);
}

// 创建顶点和索引信息，并设置GPU的缓存
void ShapesDemo::BuildGeometryBuffers()
{
	// Mesh信息
	GeometryGenerator::MeshData box;
	GeometryGenerator::CreateBox(1.0f, 1.0f, 1.0f, box);

	GeometryGenerator::MeshData grid;
	GeometryGenerator::CreateGrid(20.0f, 30.0f, 60, 40, grid);

	GeometryGenerator::MeshData sphere;
	GeometryGenerator::CreateSphere(0.5f, 20, 20, sphere);

#pragma region 练习6.14

	//GeometryGenerator::CreateGeosphere(0.5f, 0, sphere);
	//GeometryGenerator::CreateGeosphere(0.5f, 1, sphere);
	//GeometryGenerator::CreateGeosphere(0.5f, 2, sphere);
	//GeometryGenerator::CreateGeosphere(0.5f, 3, sphere);

#pragma endregion

	GeometryGenerator::MeshData cylinder;
	GeometryGenerator::CreateCylinder(0.5f, 0.3f, 3.0f, 20, 20, cylinder);

	// 顶点、偏移信息
	mBoxVertexOffset = 0;
	mGridVertexOffset = box.Vertices.size();
	mSphereVertexOffset = mGridVertexOffset + grid.Vertices.size();
	mCylinderVertexOffset = mSphereVertexOffset + sphere.Vertices.size();

	mBoxIndexCount = box.Indices.size();
	mGridIndexCount = grid.Indices.size();
	mSphereIndexCount = sphere.Indices.size();
	mCylinderIndexCount = cylinder.Indices.size();

	mBoxIndexOffset = 0;
	mGridIndexOffset = mBoxIndexCount;
	mSphereIndexOffset = mGridIndexOffset + mGridIndexCount;
	mCylinderIndexOffset = mSphereIndexOffset + mSphereIndexCount;

	totalVertexCount =
		box.Vertices.size()
		+ grid.Vertices.size()
		+ sphere.Vertices.size()
		+ cylinder.Vertices.size();
	totalIndexCount =
		mBoxIndexCount
		+ mGridIndexCount
		+ mSphereIndexCount
		+ mCylinderIndexCount;

	// 添加顶点信息
	std::vector<Vertex> vertices(totalVertexCount);
	XMFLOAT4 black(0.0f, 0.0f, 0.0f, 1.0f);

	UINT k = 0;	// 每个顶点都偏移k，但因为做的分别渲染，索引信息并不需要偏移k
	for (size_t i = 0; i < box.Vertices.size(); i++, k++)
	{
		vertices[k].Pos = box.Vertices[i].Position;
		vertices[k].Color = black;
	}

	for (size_t i = 0; i < grid.Vertices.size(); i++, k++)
	{
		vertices[k].Pos = grid.Vertices[i].Position;
		vertices[k].Color = black;
	}

	for (size_t i = 0; i < sphere.Vertices.size(); i++, k++)
	{
		vertices[k].Pos = sphere.Vertices[i].Position;
		vertices[k].Color = black;
	}

	for (size_t i = 0; i < cylinder.Vertices.size(); i++, k++)
	{
		vertices[k].Pos = cylinder.Vertices[i].Position;
		vertices[k].Color = black;
	}

	// 添加索引信息
	std::vector<UINT> indices;
	indices.insert(indices.end(), box.Indices.begin(), box.Indices.end());
	indices.insert(indices.end(), grid.Indices.begin(), grid.Indices.end());
	indices.insert(indices.end(), sphere.Indices.begin(), sphere.Indices.end());
	indices.insert(indices.end(), cylinder.Indices.begin(), cylinder.Indices.end());


	// 顶点缓存的描述
	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D11_USAGE_DEFAULT;
	vbd.ByteWidth = sizeof(Vertex)*totalVertexCount;
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

	// 索引缓存的描述
	D3D11_BUFFER_DESC ibd;
	ZeroMemory(&ibd, sizeof(ibd));
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.ByteWidth = sizeof(UINT) * totalIndexCount;
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
void ShapesDemo::BuildFX()
{
	ID3DBlob* blob;

	// 创建顶点着色器
	HR(D3DUtil::CreateShaderFromFile(L"HLSL\\Shapes_VS.vso", L"HLSL\\Shapes_VS.hlsl", "VS", "vs_5_0", &blob));
	HR(md3dDevice->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &mVertexShader));

	// 创建顶点布局
	D3D11_INPUT_ELEMENT_DESC vertexLayout[2] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	HR(md3dDevice->CreateInputLayout(vertexLayout, ARRAYSIZE(vertexLayout), blob->GetBufferPointer(), blob->GetBufferSize(), &mVertexLayout));
	md3dImmediateContext->IASetInputLayout(mVertexLayout);// 设定输入布局
	ReleaseCOM(blob);

	// 创建像素着色器
	HR(D3DUtil::CreateShaderFromFile(L"HLSL\\Shapes_PS.pso", L"HLSL\\Shapes_PS.hlsl", "PS", "ps_5_0", &blob));
	HR(md3dDevice->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &mPixelShader));
	ReleaseCOM(blob);

	md3dImmediateContext->VSSetShader(mVertexShader, nullptr, 0);
	md3dImmediateContext->PSSetShader(mPixelShader, nullptr, 0);
}

void ShapesDemo::UpdateScene(float dt)
{
	float x = mRadius * sinf(mPhi) * cosf(mTheta);
	float z = mRadius * sinf(mPhi) * sinf(mTheta);
	float y = mRadius * cosf(mPhi);

	XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX V = XMMatrixLookAtLH(pos, target, up);
	DirectX::XMStoreFloat4x4(&mView, V);
}

void ShapesDemo::DrawScene()
{
	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::LightSteelBlue));
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	//设定为线框方式渲染
	md3dImmediateContext->RSSetState(mWireframeRS);	

#pragma region 练习6.13

	//D3D11_RECT rects = { 100, 100, 400, 400 };
	//md3dImmediateContext->RSSetScissorRects(1, &rects);

#pragma endregion

	// vp矩阵不变
	DirectX::XMMATRIX view = DirectX::XMLoadFloat4x4(&mView);
	DirectX::XMMATRIX proj = DirectX::XMLoadFloat4x4(&mProj);
	DirectX::XMMATRIX vp = view * proj;

	// world矩阵根据不同的物体而不同
	// 渲染地面
	DirectX::XMMATRIX world = DirectX::XMLoadFloat4x4(&mGridWorld);
	mCBuffer.wvp = world * view * proj;
	md3dImmediateContext->UpdateSubresource(mBoxCB, 0, nullptr, &mCBuffer, 0, 0);
	md3dImmediateContext->DrawIndexed(mGridIndexCount, mGridIndexOffset, mGridVertexOffset);

	// 渲染盒子
	world = DirectX::XMLoadFloat4x4(&mBoxWorld);
	mCBuffer.wvp = world * view * proj;
	md3dImmediateContext->UpdateSubresource(mBoxCB, 0, nullptr, &mCBuffer, 0, 0);
	md3dImmediateContext->DrawIndexed(mBoxIndexCount, mBoxIndexOffset, mBoxVertexOffset);

	// 渲染中间球
	world = DirectX::XMLoadFloat4x4(&mCenterSphere);
	mCBuffer.wvp = world * view * proj;
	md3dImmediateContext->UpdateSubresource(mBoxCB, 0, nullptr, &mCBuffer, 0, 0);
	md3dImmediateContext->DrawIndexed(mSphereIndexCount, mSphereIndexOffset, mSphereVertexOffset);

	// 渲染10个柱子
	for (size_t i = 0; i < 10; i++)
	{
		world = DirectX::XMLoadFloat4x4(&mCylWorld[i]);
		mCBuffer.wvp = world * view * proj;
		md3dImmediateContext->UpdateSubresource(mBoxCB, 0, nullptr, &mCBuffer, 0, 0);
		md3dImmediateContext->DrawIndexed(mCylinderIndexCount, mCylinderIndexOffset, mCylinderVertexOffset);
	}

	// 渲染10个柱子上的球
	for (size_t i = 0; i < 10; i++)
	{
		world = DirectX::XMLoadFloat4x4(&mSphereWorld[i]);
		mCBuffer.wvp = world * view * proj;
		md3dImmediateContext->UpdateSubresource(mBoxCB, 0, nullptr, &mCBuffer, 0, 0);
		md3dImmediateContext->DrawIndexed(mSphereIndexCount, mSphereIndexOffset, mSphereVertexOffset);
	}

	HR(mSwapChain->Present(0, 0));
}

void ShapesDemo::OnMouseDown(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;
	SetCapture(mhMainWnd);	// 捕获鼠标
}

void ShapesDemo::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void ShapesDemo::OnMouseMove(WPARAM btnState, int x, int y)
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
		mRadius = MathHelper::Clamp(mRadius, 3.0f, 50.0f);
	}

	mLastMousePos.x = x;
	mLastMousePos.y = y;
}
