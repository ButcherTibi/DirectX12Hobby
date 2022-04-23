struct VertexOut
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

[RootSignature(RootSign)]
float4 main(VertexOut input) : SV_TARGET
{
    return input.color;
}