struct GPU_IndexUpdateGroup {
	uint poly_idx[12];
	uint new_vertex_idx[12][6];
};
StructuredBuffer<GPU_IndexUpdateGroup> updates;

RWStructuredBuffer<uint> indexes;


[numthreads(12, 6, 1)]
void main(uint3 group_ids : SV_GroupID, uint3 thread_ids : SV_GroupThreadID)
{
	GPU_IndexUpdateGroup update = updates.Load(group_ids.x);

	uint poly_idx = update.poly_idx[thread_ids.x];
    uint new_vertex_idx = update.new_vertex_idx[thread_ids.x][thread_ids.y];

    if (poly_idx != 0) {
        uint idx = 6 * (poly_idx - 1);
        indexes[idx + thread_ids.y] = new_vertex_idx + 1;
    }
}