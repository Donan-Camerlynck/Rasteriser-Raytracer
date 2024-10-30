// ----------------------------------------------------------------------------
// Global variables
// ----------------------------------------------------------------------------

float4x4 gWorldViewProj : WorldViewProjection;
Texture2D gDiffuseMap : DiffuseMap;
Texture2D gNormalMap : NormalMap;
Texture2D gSpecularMap : SpecularMap;
Texture2D gGlossinessMap : GlossinessMap;
float4x4 gWorldMatrix : World;
float4x4 gViewInv : ViewInverse;

float3 gLightDir = normalize(float3(0.577f, -0.577f, 0.577f));

float gLightIntensity = 7.0f;
float gShininess = 25.0f;
float gPI = 3.14159f;

// ----------------------------------------------------------------------------
// sampler
// ----------------------------------------------------------------------------
SamplerState samPoint
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = Wrap; //or Mirror, Clamp, Border
	AddressV = Wrap; //or Mirror, Clamp, Border
};

SamplerState samLinear
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap; //or Mirror, Clamp, Border
	AddressV = Wrap; //or Mirror, Clamp, Border
};

SamplerState samAnisotropic
{
	Filter = ANISOTROPIC;
	AddressU = Wrap; //or Mirror, Clamp, Border
	AddressV = Wrap; //or Mirror, Clamp, Border
};

//-----------------------------------------------------------------------------
// Input/Output Structs
//-----------------------------------------------------------------------------
struct VS_INPUT
{
	float3 Position : POSITION;
	float2 Uv : TEXCOORD;
	float3 Normal : NORMAL;
	float3 Tangent : TANGENT;
};

struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
	float4 WorldPosition : COLOR;
	float2 Uv : TEXCOORD;
	float3 Normal : NORMAL;
	float3 Tangent : TANGENT;
};

//-----------------------------------------------------------------------------
// Vertex Shader
//-----------------------------------------------------------------------------
VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	output.Position = mul(float4(input.Position, 1.f), gWorldViewProj);
	output.WorldPosition = mul(output.Position, gWorldMatrix);
	output.Uv = input.Uv;
	output.Normal = mul(normalize(input.Normal), (float3x3)gWorldMatrix);
	output.Tangent = mul(normalize(input.Tangent), (float3x3)gWorldMatrix);
	return output;
}

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------
float4 Lambert(float kd, float4 cd)
{
	return (kd * cd / gPI);
}

float Phong(float ks, float exp, float3 l, float3 v, float3 n)
{
	float3 ref = reflect(l, n);
	float alpha = saturate(dot(ref, v));
	return  (ks * pow(alpha, exp));
}

//-----------------------------------------------------------------------------
// Pixel Shader
//-----------------------------------------------------------------------------
float4 PS(VS_OUTPUT input, SamplerState state) : SV_TARGET
{
	float3 binormal = normalize(cross(input.Normal,input.Tangent));
	float4x4 tangentSpaceAxis = float4x4(float4(input.Tangent, 0.f), float4(binormal, 0.f), float4(input.Normal, 0.f), float4(0.0f, 0.0f, 0.0f, 1.0f));

	float3 normalSample = gNormalMap.Sample(state, input.Uv).rgb;
	normalSample = 2 * normalSample - float3(1.f, 1.f,  1.f);
	normalSample = mul(float4(normalSample, 0.f), tangentSpaceAxis);

	float observedArea = saturate(dot(normalSample, -gLightDir));

	float3 viewDirection = normalize(input.WorldPosition.xyz - gViewInv[3].xyz);

	float4 diffuse = Lambert(gLightIntensity, gDiffuseMap.Sample(state, input.Uv));
	float specular = Phong(gSpecularMap.Sample(state, input.Uv), gShininess * gGlossinessMap.Sample(state, input.Uv).r, gLightDir, -viewDirection, normalSample);
	return float4((diffuse + specular) * observedArea + float4(0.025f, 0.025f, 0.025f, 0.f));
}

float4 PS_samPoint(VS_OUTPUT input) : SV_TARGET
{
	return PS(input,samPoint);
}

float4 PS_samLinear(VS_OUTPUT input) : SV_TARGET
{
	return PS(input,samLinear);
}

float4 PS_samAnisotropic(VS_OUTPUT input) : SV_TARGET
{
	return PS(input,samAnisotropic);
}


//-----------------------------------------------------------------------------
// Technique
//-----------------------------------------------------------------------------
technique11 PointFilterTechnique
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_samPoint()));
	}
}

technique11 LinearFilterTechnique
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_samLinear()));
	}
}

technique11 AnisotropicFilterTechnique
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, PS_samAnisotropic()));

	}
}