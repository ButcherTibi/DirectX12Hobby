#include "../CommonTypes.hlsli"

ConstantBuffer<Frame> frame : register(b0);
Texture2D tex : register(t0);

//RootSignature
float4 main(float4 pos : SV_POSITION) : SV_TARGET
{
	float4 pix = tex.Load(int3(pos.xy, 0));

	if (pix.a == 0) {
		return float4(0, 0, 0, 0.75);
	}
	return pix;
}