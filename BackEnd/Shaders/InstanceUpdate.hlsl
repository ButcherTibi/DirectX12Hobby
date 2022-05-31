struct GPU_Instance {
	// Transform
	float3 pos;
	float4 rot;
	float3 scale;

	// Material
	float3 albedo_color;
	float roughness;
	float metallic;
	float specular;

	// Wireframe
	float3 wireframe_front_color;
	float4 wireframe_back_color;
	float3 wireframe_tess_front_color;
	float4 wireframe_tess_back_color;
	float wireframe_tess_split_count;
	float wireframe_tess_gap;
};

struct GPU_InstanceUpdateGroup {
	uint instance_idx[12];
	GPU_Instance new_data[12];
};
StructuredBuffer<GPU_InstanceUpdateGroup> updates;

RWStructuredBuffer<GPU_Instance> instances;


[numthreads(12, 1, 1)]
void main(uint3 group_ids : SV_GroupID, uint3 thread_ids : SV_GroupThreadID)
{
	GPU_InstanceUpdateGroup update = updates.Load(group_ids.x);

	uint instance_idx = update.instance_idx[thread_ids.x];
	GPU_Instance new_data = update.new_data[thread_ids.x];

    if (instance_idx != 0) {
        instances[instance_idx - 1] = new_data;
    }
}