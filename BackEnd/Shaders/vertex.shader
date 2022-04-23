
struct VertexOut
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

struct Vertex
{
    float4 position : SV_POSITION;
    float4 normal : NORMAL;
};

StructuredBuffer<Vertex> vertices : register(t0);

[RootSignature(RootSign)]
VertexOut main(
    uint vertex_idx : SV_VertexID)
{
    VertexOut result;

    VertexOut verts[3];
    verts[0].position.xyz = float3( 0.0f, 0.25f, 0.0f );
    verts[1].position.xyz = float3( 0.25f, -0.25f, 0.0f );
    verts[2].position.xyz = float3( -0.25f, -0.25f, 0.0f );

    verts[0].color = float4(1.0f, 0.0f, 0.0f, 1.0f);
    verts[1].color = float4(0.0f, 1.0f, 0.0f, 1.0f);
    verts[2].color = float4(0.0f, 0.0f, 1.0f, 1.0f);

    VertexOut vert = verts[vertex_idx];

    result.position = vertices[vertex_idx].position;
    result.position.w = 0.5;
    result.color = float4(1.0f, 0.0f, 0.0f, 1.0f);

    return result;
}
