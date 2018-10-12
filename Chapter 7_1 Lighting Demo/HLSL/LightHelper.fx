
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
	// 初始化输出
	ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// 环境光的计算
	ambient = mat.Ambient * L.Ambient;

	float3 lightVec = -L.Direction;	// 获取light的向量
	float diffuseFactor = dot(lightVec, normal);	// 漫反射的强度

	[flatten]
	if (diffuseFactor > 0.0f)
	{
		// 漫反射光的计算
		diffuse = diffuseFactor * mat.Diffuse * L.Diffuse;

		// 高光的计算
		float3 v = reflect(-lightVec, normal);	// 计算反射角
		float specFactor = pow(max(dot(v, toEye), 0.0f), mat.Specular.w);	// 高光的衰减程度
		spec = specFactor * mat.Specular * L.Specular;

		// Blinn的方式
		//float3 h = normalize(toEye + lightVec);
		//float specFactor = pow(max(dot(h, normal), 0.0f), mat.Specular.w);
		//spec = specFactor * mat.Specular * L.Specular;
	}
}

void ComputePointLight(Material mat, PointLight L,
	float3 pos, float3 normal, float3 toEye,
	out float4 ambient, out float4 diffuse, out float4 spec)
{
	// 初始化输出
	ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

	float3 lightVec = L.Position - pos;	// 获取light的向量
	float d = length(lightVec);

	// 超过范围直接跳出
	if (d > L.Range)
		return;

	// 环境光的计算
	ambient = mat.Ambient * L.Ambient;

	lightVec /= d;
	float diffuseFactor = dot(lightVec, normal);	// 漫反射的强度

	[flatten]
	if (diffuseFactor > 0.0f)
	{
		// 漫反射光的计算
		diffuse = diffuseFactor * mat.Diffuse * L.Diffuse;

		// 高光的计算
		float3 v = reflect(-lightVec, normal);	// 计算反射角
		float specFactor = pow(max(dot(v, toEye), 0.0f), mat.Specular.w);	// 高光的衰减程度
		spec = specFactor * mat.Specular * L.Specular;

		// Blinn的方式
		//float3 h = normalize(toEye + lightVec);
		//float specFactor = pow(max(dot(h, normal), 0.0f), mat.Specular.w);
		//spec = specFactor * mat.Specular * L.Specular;

		// 衰减
		float att = 1.0f / dot(L.Att, float3(1.0f, d, d*d));

		diffuse *= att;
		spec *= att;
	}
}

void ComputeSpotLight(Material mat, SpotLight L,
	float3 pos, float3 normal, float3 toEye,
	out float4 ambient, out float4 diffuse, out float4 spec)
{
	// 初始化输出
	ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	spec = float4(0.0f, 0.0f, 0.0f, 0.0f);

	float3 lightVec = L.Position - pos;	// 获取light的向量
	float d = length(lightVec);

	if (d > L.Range)
		return;

	// 环境光的计算
	ambient = mat.Ambient * L.Ambient;
	float spot = pow(max(dot(-lightVec, L.Direction), 0.0f), L.Spot);
	ambient *= spot;

	lightVec /= d;
	float diffuseFactor = dot(lightVec, normal);	// 漫反射的强度

	[flatten]
	if (diffuseFactor > 0.0f)
	{
		// 漫反射光的计算
		diffuse = diffuseFactor * mat.Diffuse * L.Diffuse;

		// 高光的计算
		float3 v = reflect(-lightVec, normal);	// 计算反射角
		float specFactor = pow(max(dot(v, toEye), 0.0f), mat.Specular.w);	// 高光的衰减程度
		spec = specFactor * mat.Specular * L.Specular;

		// Blinn的方式
		//float3 h = normalize(toEye + lightVec);
		//float specFactor = pow(max(dot(h, normal), 0.0f), mat.Specular.w);
		//spec = specFactor * mat.Specular * L.Specular;

		float att = spot / dot(L.Att, float3(1.0f, d, d * d));
		diffuse *= att;
		spec *= att;
	}
}










