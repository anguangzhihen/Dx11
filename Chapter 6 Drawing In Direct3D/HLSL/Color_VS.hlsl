// 顶点着色器
#include "Color.fx"

VertexOut VS(VertexIn pIn)
{
	// 练习6.6 shader在重新生成时，记得将vso和pso文件给删除 
	//pIn.PosL.xy += 0.5f*sin(pIn.PosL.x)*sin(3.0f*gTime);
	//pIn.PosL.z *= 0.6f + 0.4f*sin(2.0f*gTime);

	VertexOut pOut;
	pOut.PosH = mul(float4(pIn.PosL, 1.0f), wvp);

	pOut.Color = pIn.Color;
	return pOut;
}