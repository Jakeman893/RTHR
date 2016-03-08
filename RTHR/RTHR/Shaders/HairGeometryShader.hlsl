struct GSOutput
{
	float4 pos : SV_POSITION;
};

[maxvertexcount(3)]
void main(
	line float4 input[2] : SV_POSITION, 
	inout LineStream<GSOutput> output
)
{
	for (uint i = 0; i < 2; i++)
	{
		GSOutput element;
		element.pos = input[i];
		output.Append(element);
	}
}