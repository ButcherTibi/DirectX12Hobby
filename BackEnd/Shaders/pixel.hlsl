#include "CommonTypes.hlsli"

struct PixelOutput {
	float4 world_pos : SV_TARGET0;
	float4 color : SV_TARGET1;
};

//RootSignature
PixelOutput main(VertexOut input)
{
	PixelOutput result;
	result.world_pos = float4(input.world_pos, 0.1234f);
	result.color = float4(1.0f, 0.0f, 0.0f, 1.0f);
    return result;
}