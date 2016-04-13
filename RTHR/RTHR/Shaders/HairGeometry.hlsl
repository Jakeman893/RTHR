#include "HairHeader.hlsli"

[maxvertexcount(6)]
void main(
	line GSInput points[2], 
	inout TriangleStream< GSOutput > output
)
{
	float4 p0 = points[0].pos;
	float4 p1 = points[1].pos;

	float w0 = p0.w;
	float w1 = p1.w;

	float3 dir = points[0].norm;

	p0.xyz /= p0.w;
	p1.xyz /= p1.w;

	// Scale to window aspect ratio
	float3 ratio = float3(RenderTargetSize.y, RenderTargetSize.x, 0);
	ratio = normalize(ratio);

	float3 unit_z = normalize(float3(0, 0, -1));

	float3 normal = normalize(cross(unit_z, dir) * ratio);

	float width = 0.005;

	GSOutput v[4];

	float3 dir_offset = dir * ratio * width;
	float3 normal_scaled = normal * ratio * width;

	float3 p0_ex = p0 - dir_offset;
	float3 p1_ex = p1 + dir_offset;

	v[0].pos = float4(p0_ex - normal_scaled, 1) * w0;
	v[0].color = points[0].color;

	v[1].pos = float4(p0_ex + normal_scaled, 1) * w0;
	v[1].color = points[0].color;

	v[2].pos = float4(p1_ex + normal_scaled, 1) * w1;
	v[2].color = points[1].color;

	v[3].pos = float4(p1_ex - normal_scaled, 1) * w1;
	v[3].color = points[1].color;

	output.Append(v[2]);
	output.Append(v[1]);
	output.Append(v[0]);

	output.RestartStrip();

	output.Append(v[3]);
	output.Append(v[2]);
	output.Append(v[0]);

	output.RestartStrip();
}