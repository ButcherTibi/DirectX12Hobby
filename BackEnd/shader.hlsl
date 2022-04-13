struct PixelInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

PixelInput VertexMain(float4 position : POSITION, float4 color : COLOR)
{
    PixelInput result;

    PixelInput verts[3];
    verts[0].position = float4( 0.0f, 0.25f, 0.0f, 0.f );
    verts[1].position = float4( 0.25f, -0.25f, 0.0f, 0.f );
    verts[2].position = float4( -0.25f, -0.25f, 0.0f, 0.f );

    // { { 0.0f, 0.25f * 1, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
    // { { 0.25f, -0.25f * 1, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
    // { { -0.25f, -0.25f * 1, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } }

    result.position = position;
    result.color = color;

    return result;
}

float4 PixelMain(PixelInput input) : SV_TARGET
{
    return input.color;
}
