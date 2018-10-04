// ������ɫ��
#include "Color.fx"

VertexOut VS(VertexIn pIn)
{
	// ��ϰ6.6 shader����������ʱ���ǵý�vso��pso�ļ���ɾ�� 
	//pIn.PosL.xy += 0.5f*sin(pIn.PosL.x)*sin(3.0f*gTime);
	//pIn.PosL.z *= 0.6f + 0.4f*sin(2.0f*gTime);

	VertexOut pOut;
	pOut.PosH = mul(float4(pIn.PosL, 1.0f), wvp);

	pOut.Color = pIn.Color;
	return pOut;
}