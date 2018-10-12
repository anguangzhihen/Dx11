
struct DirectionalLight
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;
	float3 Direction;
	float pad;
};

struct PointLight 
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;

	float3 Position;
	float Range;

	float3 Att;
	float pad;
};

struct SpotLight
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;

	float3 Position;
	float Range;

	float3 Direction;
	float Spot;

	float3 Att;
	float pad;
};

struct Material
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;	// w = SpecPower
	float4 Reflect;
};

void ComputeDirectionalLight(Material mat, DirectionalLight L,
	float3 normal, float3 toEye,
	out float4 ambient, out float4 diffuse, out float4 spec)
{
	// ��ʼ�����
	ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// ������ļ���
	ambient = mat.Ambient * L.Ambient;

	float3 lightVec = -L.Direction;	// ��ȡlight������
	float diffuseFactor = dot(lightVec, normal);	// �������ǿ��

	[flatten]
	if (diffuseFactor > 0.0f)
	{
		// �������ļ���
		diffuse = diffuseFactor * mat.Diffuse * L.Diffuse;

		// �߹�ļ���
		float3 v = reflect(-lightVec, normal);	// ���㷴���
		float specFactor = pow(max(dot(v, toEye), 0.0f), mat.Specular.w);	// �߹��˥���̶�
		spec = specFactor * mat.Specular * L.Specular;

		// Blinn�ķ�ʽ
		//float3 h = normalize(toEye + lightVec);
		//float specFactor = pow(max(dot(h, normal), 0.0f), mat.Specular.w);
		//spec = specFactor * mat.Specular * L.Specular;
	}
}

void ComputePointLight(Material mat, PointLight L,
	float3 pos, float3 normal, float3 toEye,
	out float4 ambient, out float4 diffuse, out float4 spec)
{
	// ��ʼ�����
	ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

	float3 lightVec = L.Position - pos;	// ��ȡlight������
	float d = length(lightVec);

	// ������Χֱ������
	if (d > L.Range)
		return;

	// ������ļ���
	ambient = mat.Ambient * L.Ambient;

	lightVec /= d;
	float diffuseFactor = dot(lightVec, normal);	// �������ǿ��

	[flatten]
	if (diffuseFactor > 0.0f)
	{
		// �������ļ���
		diffuse = diffuseFactor * mat.Diffuse * L.Diffuse;

		// �߹�ļ���
		float3 v = reflect(-lightVec, normal);	// ���㷴���
		float specFactor = pow(max(dot(v, toEye), 0.0f), mat.Specular.w);	// �߹��˥���̶�
		spec = specFactor * mat.Specular * L.Specular;

		// Blinn�ķ�ʽ
		//float3 h = normalize(toEye + lightVec);
		//float specFactor = pow(max(dot(h, normal), 0.0f), mat.Specular.w);
		//spec = specFactor * mat.Specular * L.Specular;

		// ˥��
		float att = 1.0f / dot(L.Att, float3(1.0f, d, d*d));

		diffuse *= att;
		spec *= att;
	}
}

void ComputeSpotLight(Material mat, SpotLight L,
	float3 pos, float3 normal, float3 toEye,
	out float4 ambient, out float4 diffuse, out float4 spec)
{
	// ��ʼ�����
	ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

	float3 lightVec = L.Position - pos;	// ��ȡlight������
	float d = length(lightVec);

	if (d > L.Range)
		return;

	// ������ļ���
	ambient = mat.Ambient * L.Ambient;
	float spot = pow(max(dot(-lightVec, L.Direction), 0.0f), L.Spot);
	ambient *= spot;

	lightVec /= d;
	float diffuseFactor = dot(lightVec, normal);	// �������ǿ��

	[flatten]
	if (diffuseFactor > 0.0f)
	{
		// �������ļ���
		diffuse = diffuseFactor * mat.Diffuse * L.Diffuse;

		// �߹�ļ���
		float3 v = reflect(-lightVec, normal);	// ���㷴���
		float specFactor = pow(max(dot(v, toEye), 0.0f), mat.Specular.w);	// �߹��˥���̶�
		spec = specFactor * mat.Specular * L.Specular;

		// Blinn�ķ�ʽ
		//float3 h = normalize(toEye + lightVec);
		//float specFactor = pow(max(dot(h, normal), 0.0f), mat.Specular.w);
		//spec = specFactor * mat.Specular * L.Specular;

		float att = spot / dot(L.Att, float3(1.0f, d, d * d));
		diffuse *= att;
		spec *= att;
	}
}










