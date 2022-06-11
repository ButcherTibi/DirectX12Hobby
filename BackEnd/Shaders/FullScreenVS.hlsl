
static float2 verts[3] = {
	float2(-1, -1),
	float2(-1, 2),
	float2(2, -1)
};

float4 main(uint vertex_id : SV_VertexID) : SV_POSITION
{
	return float4(
		verts[vertex_id].x,
		verts[vertex_id].y,
		0,
		0.5
	);
}