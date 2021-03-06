sampler Sampler : register(s0);

// Per-pixel color data passed through the pixel shader.
struct GSOutput
{
	float4 pos : SV_POSITION;
	float3 color : COLOR0;
};

// A pass-through function for the (interpolated) color data.
float4 main(GSOutput input) : SV_TARGET
{
	//TODO: Add phong model here
	return float4(input.color, 1.0f);
}