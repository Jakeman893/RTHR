#include "HairHeader.hlsli"

// Simple shader to do vertex processing on the GPU.
GSInput main(VertexShaderInput input)
{
	GSInput output;
	float4 pos = float4(input.pos, 1.0f);

	// Transform the vertex position into projected space.
	output.pos = mul(pos, WorldViewProj);

	// Pass the color through without modification.
	output.color = DiffuseColor;

	// Per-pixel normal
	output.norm = input.norm;

	// Uncomment for per-vertex color
	//output.color = input.color;

	return output;
}