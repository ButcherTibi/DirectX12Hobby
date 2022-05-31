// #include "Math.hlsli"

struct VertexOut
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

struct Vertex
{
    float3 position;
    float3 normal;
};

StructuredBuffer<Vertex> vertices : register(t0);

//RootSignature
VertexOut main(uint vertex_idx : SV_VertexID)
{
    VertexOut result;
    result.position.xyz = vertices[vertex_idx].position;
    result.position.w = 0.5;
    result.color = float4(1.0f, 0.0f, 0.0f, 1.0f);

    return result;
}
