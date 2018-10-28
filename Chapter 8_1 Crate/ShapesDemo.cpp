#include "ShapesDemo.h"
#include "d3dUtil.h"
#include "MathHelper.h"
#include "LightHelper.h"
#include "GeometryGenerator.h"

#include <filesystem>

using namespace DirectX;
using namespace std::experimental;

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
mVertexShader(0), mPixelShader(0), mVertexLayout(0),
mBoxVB(0), mBoxIB(0), mBoxCB(0), mFrameBoxCB(0),
mWoodCrateDDS(0), mSamplerState(0),
mTheta(1.5f * MathHelper::Pi), mPhi(0.25f*MathHelper::Pi), mRadius(5.0f)
{
	mMainWndCaption = L"Box Demo";

	mLastMousePos.x = 0;
	mLastMousePos.y = 0;

	XMMATRIX I = XMMatrixIdentity();

	// 默认为单位矩阵
	// 行主矩阵，单精度浮点值
	XMStoreFloat4x4(&mView, I);
	XMStoreFloat4x4(&mProj, I);
	XMStoreFloat4x4(&mTexTransform, I);

	XMMATRIX boxScale = XMMatrixScaling(1.0f, 1.0f, 1.0f);
	XMMATRIX boxOffset = XMMatrixTranslation(0.0f, 0.0f, 0.0f);
	XMStoreFloat4x4(&mBoxWorld, XMMatrixMultiply(boxScale, boxOffset));

	// 直射光
	mDirLight.Ambient = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	mDirLight.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mDirLight.Specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	mDirLight.Direction = XMFLOAT3(0.4f, -1.0f, 0.4f);

	mMat.Ambient = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mMat.Diffuse = XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f);
	mMat.Specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 96.0f);
}

ShapesDemo::~ShapesDemo()
{
	ReleaseCOM(mBoxVB);
	ReleaseCOM(mBoxIB);
	ReleaseCOM(mBoxCB);
	ReleaseCOM(mFrameBoxCB);
	ReleaseCOM(mVertexLayout);
	ReleaseCOM(mVertexShader);
	ReleaseCOM(mPixelShader);
	ReleaseCOM(mWoodCrateDDS);
	ReleaseCOM(mSamplerState);
}

bool ShapesDemo::Init()
{
	if (!D3DApp::Init())
		return false;

	// 获取木箱子纹理
	HR(CreateDDSTextureFromFile(md3dDevice, L"Textures\\WoodCrate01.dds", nullptr, &mWoodCrateDDS));

	// 初始化采样器状态
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	//sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;	// 过滤方法设置为各向异性
	sampDesc.MaxAnisotropy = 4;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;	// 压缩方法
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	HR(md3dDevice->CreateSamplerState(&sampDesc, &mSamplerState));

	BuildGeometryBuffers();
	BuildFX();

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

	// 顶点、偏移信息
	mBoxVertexOffset = 0;
	mBoxIndexCount = box.Indices.size();
	mBoxIndexOffset = 0;

	totalVertexCount =
		box.Vertices.size();
	totalIndexCount =
		mBoxIndexCount;

	// 添加顶点信息
	std::vector<Vertex> vertices(totalVertexCount);
	XMFLOAT4 black(0.0f, 0.0f, 0.0f, 1.0f);

	UINT k = 0;	// 每个顶点都偏移k，但因为做的分别渲染，索引信息并不需要偏移k
	for (size_t i = 0; i < box.Vertices.size(); i++, k++)
	{
		vertices[k].Pos = box.Vertices[i].Position;
		vertices[k].Normal = box.Vertices[i].Normal;
		vertices[k].Tex = box.Vertices[i].TexC;
	}

	// 添加索引信息
	std::vector<UINT> indices;
	indices.insert(indices.end(), box.Indices.begin(), box.Indices.end());

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
	md3dImmediateContext->VSSetConstantBuffers(0, 1, &mBoxCB);	// VS常量缓冲区对应b0

	D3D11_BUFFER_DESC cbdFrame;
	ZeroMemory(&cbdFrame, sizeof(cbdFrame));
	cbdFrame.Usage = D3D11_USAGE_DEFAULT;
	cbdFrame.ByteWidth = sizeof(FrameConstantBuffer);
	cbdFrame.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbdFrame.CPUAccessFlags = 0;
	HR(md3dDevice->CreateBuffer(&cbdFrame, nullptr, &mFrameBoxCB));
	md3dImmediateContext->PSSetConstantBuffers(1, 1, &mFrameBoxCB);	// PS常量缓冲区对应b1
}

// 设置着色器
void ShapesDemo::BuildFX()
{
	ID3DBlob* blob;

	// 创建顶点着色器
	HR(D3DUtil::CreateShaderFromFile(L"HLSL\\Shapes_VS.vso", L"HLSL\\Shapes_VS.hlsl", "VS", "vs_5_0", &blob));
	HR(md3dDevice->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &mVertexShader));

	// 创建顶点布局
	D3D11_INPUT_ELEMENT_DESC vertexLayout[3] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 }
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

	mEyePosW = XMFLOAT4(x, y, z, 0.0f);

	XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX V = XMMatrixLookAtLH(pos, target, up);
	DirectX::XMStoreFloat4x4(&mView, V);

	// 片元缓存
	mFrameCBuffer.gEyePosW = mEyePosW;
	mFrameCBuffer.gDirLight = mDirLight;
	mFrameCBuffer.gMaterial = mMat;
	md3dImmediateContext->UpdateSubresource(mFrameBoxCB, 0, nullptr, &mFrameCBuffer, 0, 0);

	// 片元着色器阶段设置采样器 
	md3dImmediateContext->PSSetSamplers(0, 1, &mSamplerState);	// 对应Shader中的register(s0)
	md3dImmediateContext->PSSetShaderResources(0, 1, &mWoodCrateDDS);	// 对应Shader中的regiser(t0)
}

void ShapesDemo::DrawScene()
{
	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::LightSteelBlue));
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// vp矩阵不变
	DirectX::XMMATRIX view = DirectX::XMLoadFloat4x4(&mView);
	DirectX::XMMATRIX proj = DirectX::XMLoadFloat4x4(&mProj);
	DirectX::XMMATRIX vp = view * proj;

	// 渲染盒子
	DirectX::XMMATRIX world = DirectX::XMLoadFloat4x4(&mBoxWorld);
	mCBuffer.gWorldViewProj = world * view * proj;
	mCBuffer.gWorld = world;
	mCBuffer.gWorldInvTranspose = MathHelper::InverseTranspose(world);
	mCBuffer.gTexTransform = XMLoadFloat4x4(&mTexTransform);
	md3dImmediateContext->UpdateSubresource(mBoxCB, 0, nullptr, &mCBuffer, 0, 0);
	md3dImmediateContext->DrawIndexed(mBoxIndexCount, mBoxIndexOffset, mBoxVertexOffset);

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
