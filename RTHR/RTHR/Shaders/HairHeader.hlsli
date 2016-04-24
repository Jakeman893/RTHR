// A constant buffer that stores the three basic column-major matrices for composing geometry.
cbuffer RarelyUpdated : register(b0)
{
	float3 EyePosition				: packoffset(c0);

	float4x4 World					: packoffset(c1);
	float3x3 WorldInverseTranspose	: packoffset(c5);
	float4x4 WorldViewProj			: packoffset(c8);
	float2 RenderTargetSize			: packoffset(c12);
};

cbuffer HairParams : register(b1)
{
	float4 DiffuseColor				: packoffset(c0);
}

// Per-vertex data used as input to the vertex shader.
struct VertexShaderInput
{
	float3 pos : SV_Position;
	float3 norm : NORMAL;
	float3 color : COLOR0;
};

struct GSInput
{
	float4 pos : SV_POSITION;
	float3 norm : NORMAL;
	float3 color : COLOR0;
};

struct GSOutput
{
	float4 pos : SV_POSITION;
	float3 color : COLOR0;
};