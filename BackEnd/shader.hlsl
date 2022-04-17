struct PixelInput
{
    float4 position : SV_POSITION;
    
    float4 color : COLOR;
};

PixelInput VertexMain(
    uint vertex_idx : SV_VertexID)
{
    PixelInput result;

    PixelInput verts[3];
    verts[0].position.xyz = float3( 0.0f, 0.25f, 0.0f );
    verts[1].position.xyz = float3( 0.25f, -0.25f, 0.0f );
    verts[2].position.xyz = float3( -0.25f, -0.25f, 0.0f );

    verts[0].color = float4(1.0f, 0.0f, 0.0f, 1.0f);
    verts[1].color = float4(0.0f, 1.0f, 0.0f, 1.0f);
    verts[2].color = float4(0.0f, 0.0f, 1.0f, 1.0f);

    PixelInput vert = verts[vertex_idx];

    result.position = vert.position;
    result.position.w = 0.5;
    result.color = vert.color;

    return result;
}

float4 PixelMain(PixelInput input) : SV_TARGET
{
    return input.color;
}
