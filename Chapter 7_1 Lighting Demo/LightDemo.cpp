#include "LightDemo.h"
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

	LightDemo theApp(hInstance);
	if (!theApp.Init())
		return 0;
	return theApp.Run();
}

LightDemo::LightDemo(HINSTANCE hInstance) : D3DApp(hInstance),
mVertexShader(0), mPixelShader(0), mVertexLayout(0),
mHillsVB(0), mHillsIB(0), mBoxCB(0), mFrameBoxCB(0),
mEyePosW(0.0f, 0.0f, 0.0f, 0.0f),
mWavesVB(0), mWavesIB(0),
mTheta(1.5f * MathHelper::Pi), mPhi(0.25f*MathHelper::Pi), mRadius(50.0f)
{
	mMainWndCaption = L"Hills Demo";

	mLastMousePos.x = 0;
	mLastMousePos.y = 0;

	XMMATRIX I = XMMatrixIdentity();

	// 默认为单位矩阵
	// 行主矩阵，单精度浮点值
	DirectX::XMStoreFloat4x4(&mWavesWorld, I);
	DirectX::XMStoreFloat4x4(&mHillsWorld, I);
	DirectX::XMStoreFloat4x4(&mView, I);
	DirectX::XMStoreFloat4x4(&mProj, I);

	XMMATRIX wavesOffset = XMMatrixTranslation(0.0f, -3.0f, 0.0f);
	XMStoreFloat4x4(&mWavesWorld, wavesOffset);

	// 直射光
	mDirLight.Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mDirLight.Diffuse = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mDirLight.Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mDirLight.Direction = XMFLOAT3(0.57735f, -0.57735f, 0.57735f);

	// 点光源
	mPointLight.Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	mPointLight.Diffuse = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	mPointLight.Specular = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	mPointLight.Att = XMFLOAT3(0.0f, 0.1f, 0.0f);
	mPointLight.Range = 25.0f;

	// 聚光灯
	mSpotLight.Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mSpotLight.Diffuse = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);
	mSpotLight.Specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mSpotLight.Att = XMFLOAT3(1.0f, 0.0f, 0.0f);
	mSpotLight.Spot = 96.0f;
	mSpotLight.Range = 10000.0f;

	mHillsMat.Ambient = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
	mHillsMat.Diffuse = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
	mHillsMat.Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f);

	mWavesMat.Ambient = XMFLOAT4(0.137f, 0.42f, 0.556f, 1.0f);
	mWavesMat.Diffuse = XMFLOAT4(0.137f, 0.42f, 0.556f, 1.0f);
	mWavesMat.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 96.0f);

#pragma region 练习7.1

	//mDirLight.Ambient = XMFLOAT4(0.2f, 0.0f, 0.0f, 1.0f);
	//mDirLight.Diffuse = XMFLOAT4(0.5f, 0.0f, 0.0f, 1.0f);
	//mDirLight.Specular = XMFLOAT4(0.5f, 0.0f, 0.0f, 1.0f);

	//mPointLight.Ambient = XMFLOAT4(0.0f, 0.3f, 0.0f, 1.0f);
	//mPointLight.Diffuse = XMFLOAT4(0.0f, 0.7f, 0.0f, 1.0f);
	//mPointLight.Specular = XMFLOAT4(0.0f, 0.7f, 0.0f, 1.0f);

	//mSpotLight.Ambient = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
	//mSpotLight.Diffuse = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
	//mSpotLight.Specular = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

#pragma endregion

#pragma region 练习7.2

	//mHillsMat.Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 8.0f);
	//mWavesMat.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 8.0f);
	//mHillsMat.Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 32.0f);
	//mWavesMat.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 32.0f);
	//mHillsMat.Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 64.0f);
	//mWavesMat.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 64.0f);
	//mHillsMat.Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 128.0f);
	//mWavesMat.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 128.0f);
	//mHillsMat.Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 256.0f);
	//mWavesMat.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 256.0f);

#pragma endregion

}

LightDemo::~LightDemo()
{
	ReleaseCOM(mHillsVB);
	ReleaseCOM(mHillsIB);
	ReleaseCOM(mWavesVB);
	ReleaseCOM(mWavesIB);
	ReleaseCOM(mBoxCB);
	ReleaseCOM(mFrameBoxCB);
	ReleaseCOM(mVertexLayout);
	ReleaseCOM(mVertexShader);
	ReleaseCOM(mPixelShader);
}

bool LightDemo::Init()
{
	if (!D3DApp::Init())
		return false;

	BuildWavesGemetryBuffers();
	BuildHillsGeometryBuffers();
	BuildFX();

	return true;
}

void LightDemo::OnResize()
{
	D3DApp::OnResize();

	// 当窗口尺寸变化后，重置投影矩阵
	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f * MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
	DirectX::XMStoreFloat4x4(&mProj, P);
}

void LightDemo::BuildWavesGemetryBuffers()
{
	mWaves.Init(200, 200, 0.8f, 0.03f, 3.25f, 0.4f);

	// 顶点缓存的描述
	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.ByteWidth = sizeof(Vertex) * mWaves.VertexCount();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;	// 指定CPU可写
	// 创建缓存
	HR(md3dDevice->CreateBuffer(&vbd, 0, &mWavesVB));

	std::vector<UINT> indices = mWaves.GetIndices();

	// 索引缓存的描述
	D3D11_BUFFER_DESC ibd;
	ZeroMemory(&ibd, sizeof(ibd));
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.ByteWidth = sizeof(UINT) * indices.size();
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;

	// 将索引设置到描述中
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = &(indices[0]);
	HR(md3dDevice->CreateBuffer(&ibd, &InitData, &mWavesIB));
}

// 创建顶点和索引信息，并设置GPU的缓存
void LightDemo::BuildHillsGeometryBuffers()
{
	GeometryGenerator::MeshData grid;
	GeometryGenerator::CreateGrid(160.f, 160.0f, 50, 50, grid);	// 创建160x160大小的格子数为49的Mesh

	mGridIndexCount = grid.Indices.size();

	// 提取每个顶点，赋值
	std::vector<Vertex> vertices(grid.Vertices.size());
	for (size_t i = 0; i < grid.Vertices.size(); i++)
	{
		XMFLOAT3 p = grid.Vertices[i].Position;
		p.y = GetHeight(p.x, p.z);	// 赋值高度
		vertices[i].Pos = p;
		vertices[i].Normal = GetNormal(p.x, p.z);
	}

	// 顶点缓存的描述
	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D11_USAGE_DEFAULT;
	vbd.ByteWidth = sizeof(Vertex) * grid.Vertices.size();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;

	// 将顶点设置到描述中
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = &vertices[0];

	// 创建缓存
	HR(md3dDevice->CreateBuffer(&vbd, &InitData, &mHillsVB));

	// 索引缓存的描述
	D3D11_BUFFER_DESC ibd;
	ZeroMemory(&ibd, sizeof(ibd));
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.ByteWidth = sizeof(UINT) * mGridIndexCount;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;

	// 将索引设置到描述中
	InitData.pSysMem = &(grid.Indices[0]);
	HR(md3dDevice->CreateBuffer(&ibd, &InitData, &mHillsIB));

	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	// 设定图元类型

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

// 根据x、z坐标获取高度
float LightDemo::GetHeight(float x, float z) const
{
	return 0.3f*(z*sinf(0.1f*x) + x * cosf(0.1f*z));
}

// 根据x、y坐标获取Normal
XMFLOAT3 LightDemo::GetNormal(float x, float z) const
{
	XMFLOAT3 n(
		-0.03f*z*cosf(0.1f*x) - 0.3f*cosf(0.1f*z),
		1.0f,
		-0.03f*sinf(0.1f*x) + 0.03f*x*sinf(0.1f*z));

	XMVECTOR unitNormal = XMVector3Normalize(XMLoadFloat3(&n));
	XMStoreFloat3(&n, unitNormal);
	return n;
}

// 设置着色器
void LightDemo::BuildFX()
{
	ID3DBlob* blob;

	// 创建顶点着色器
	HR(D3DUtil::CreateShaderFromFile(L"HLSL\\Color_VS.vso", L"HLSL\\Color_VS.hlsl", "VS", "vs_5_0", &blob));
	HR(md3dDevice->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &mVertexShader));

	// 创建顶点布局
	D3D11_INPUT_ELEMENT_DESC vertexLayout[2] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
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

void LightDemo::UpdateScene(float dt)
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

	// 更新Wave
	mWaves.CommonDisturbUpdate(mTimer.TotalTime(), dt);

	// 创建mappedData用以设置顶点信息
	D3D11_MAPPED_SUBRESOURCE mappedData;
	HR(md3dImmediateContext->Map(mWavesVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));

	// 更新顶点信息
	Vertex* v = reinterpret_cast<Vertex*>(mappedData.pData);
	UINT length = mWaves.VertexCount();
	for (UINT i = 0; i < length; i++)
	{
		v[i].Pos = mWaves[i];
		v[i].Normal = mWaves.Normal(i);
	}

	// 将新的顶点信息进行设置
	md3dImmediateContext->Unmap(mWavesVB, 0);

	// 更新点光源的坐标
	mPointLight.Position.x = 70.0f*cosf(0.2f*mTimer.TotalTime());
	mPointLight.Position.z = 70.0f*sinf(0.2f*mTimer.TotalTime());
	mPointLight.Position.y = MathHelper::Max(GetHeight(mPointLight.Position.x, mPointLight.Position.z), -3.0f) + 10.0f;

	// 更新聚光灯的坐标
	mSpotLight.Position = XMFLOAT3(mEyePosW.x, mEyePosW.y, mEyePosW.z);
	XMStoreFloat3(&mSpotLight.Direction, XMVector3Normalize(target - pos));

#pragma region 练习7.4 按1减少聚光灯范围 按2增加

	if (GetAsyncKeyState('1') & 0x8000)
	{
		mSpotLight.Spot += 1;
	}
	if (GetAsyncKeyState('2') & 0x8000)
	{
		mSpotLight.Spot -= 1;
	}

	if (mSpotLight.Spot < 50.0f)
		mSpotLight.Spot = 50.0f;
	else if (mSpotLight.Spot > 1000.0f)
		mSpotLight.Spot = 1000.0f;

#pragma endregion

	mFrameCBuffer.gDirLight = mDirLight;
	mFrameCBuffer.gSpotLight = mSpotLight;
	mFrameCBuffer.gPointLight = mPointLight;
	mFrameCBuffer.gEyePosW = mEyePosW;

}

void LightDemo::DrawScene()
{
	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::LightSteelBlue));
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	// 绘制Hills
	md3dImmediateContext->IASetVertexBuffers(0, 1, &mHillsVB, &stride, &offset);
	md3dImmediateContext->IASetIndexBuffer(mHillsIB, DXGI_FORMAT_R32_UINT, 0);

	// 更新顶点常量
	DirectX::XMMATRIX world = DirectX::XMLoadFloat4x4(&mHillsWorld);	// Hills位置
	DirectX::XMMATRIX view = DirectX::XMLoadFloat4x4(&mView);
	DirectX::XMMATRIX proj = DirectX::XMLoadFloat4x4(&mProj);
	mCBuffer.gWorld = world;
	mCBuffer.gWorldInvTranspose = MathHelper::InverseTranspose(world);
	mCBuffer.gWorldViewProj = world * view * proj;
	md3dImmediateContext->UpdateSubresource(mBoxCB, 0, nullptr, &mCBuffer, 0, 0);

	// 更新片元常量
	mFrameCBuffer.gMaterial = mHillsMat;	// 更新Hills的material
	md3dImmediateContext->UpdateSubresource(mFrameBoxCB, 0, nullptr, &mFrameCBuffer, 0, 0);

	md3dImmediateContext->DrawIndexed(mGridIndexCount, 0, 0);

	// 绘制Waves
	md3dImmediateContext->IASetVertexBuffers(0, 1, &mWavesVB, &stride, &offset);
	md3dImmediateContext->IASetIndexBuffer(mWavesIB, DXGI_FORMAT_R32_UINT, 0);

	// 更新顶点常量
	world = DirectX::XMLoadFloat4x4(&mWavesWorld);	// Waves位置
	view = DirectX::XMLoadFloat4x4(&mView);
	proj = DirectX::XMLoadFloat4x4(&mProj);
	mCBuffer.gWorld = world;
	mCBuffer.gWorldInvTranspose = MathHelper::InverseTranspose(world);
	mCBuffer.gWorldViewProj = world * view * proj;
	md3dImmediateContext->UpdateSubresource(mBoxCB, 0, nullptr, &mCBuffer, 0, 0);

	// 更新片元常量
	mFrameCBuffer.gMaterial = mWavesMat;	// 更新Waves的material
	md3dImmediateContext->UpdateSubresource(mFrameBoxCB, 0, nullptr, &mFrameCBuffer, 0, 0);

	md3dImmediateContext->DrawIndexed(3 * mWaves.TriangleCount(), 0, 0);

	HR(mSwapChain->Present(0, 0));
}

void LightDemo::OnMouseDown(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;
	SetCapture(mhMainWnd);	// 捕获鼠标
}

void LightDemo::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void LightDemo::OnMouseMove(WPARAM btnState, int x, int y)
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
		mRadius = MathHelper::Clamp(mRadius, 3.0f, 100.0f);
	}

	mLastMousePos.x = x;
	mLastMousePos.y = y;
}
