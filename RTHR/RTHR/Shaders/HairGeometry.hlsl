struct GSOutput
{
	float4 pos : SV_POSITION;
	float2 TexCoord : TEXCOORD;
};

struct GSInput
{
	float4 pos : SV_POSITION;
};

[maxvertexcount(6)]
void main(
	line GSInput input[2], 
	inout TriangleStream< GSOutput > output
)
{
	float4 p0 = points[0].pos;
	float4 p1 = points[1].pos;

	float w0 = p0.w;
	for (uint i = 0; i < 3; i++)
	{
		GSOutput element;
		element.pos = input[i];
		output.Append(element);
	}
}