// A constant buffer that stores the three basic column-major matrices for composing geometry.
cbuffer Parameters : register(b0)
{
	float4 DiffuseColor				: packoffset(c0);
	float3 EmissiveColor			: packoffset(c1);
	float3 SpecularColor			: packoffset(c2);
	float SpecularPower				: packoffset(c2.w);

	float3 LightDirection[3]		: packoffset(c3);
	float3 LightDiffuseColor[3]		: packoffset(c6);
	float3 LightSpecularColor[3]	: packoffset(c9);

	float3 EyePosition				: packoffset(c12);

	float4x4 World					: packoffset(c13);
	float3x3 WorldInverseTranspose	: packoffset(c17);
	float4x4 WorldViewProj			: packoffset(c20);
};

// Per-vertex data used as input to the vertex shader.
struct VertexShaderInput
{
	float3 pos : SV_Position;
	float3 color : COLOR0;
};

// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float3 color : COLOR0;
};

// Simple shader to do vertex processing on the GPU.
PixelShaderInput main(VertexShaderInput input)
{
	PixelShaderInput output;
	float4 pos = float4(input.pos, 1.0f);

	// Transform the vertex position into projected space.
	pos = mul(pos, WorldViewProj);
	output.pos = pos;

	// Pass the color through without modification.
	output.color = input.color;

	return output;
}