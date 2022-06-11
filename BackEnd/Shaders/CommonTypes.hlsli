
struct Frame {
	// Camera
	float3 camera_pos;
	float4 camera_quat_inv;
	float3 camera_forward;
	matrix perspective;
	float z_near;
	float z_far;
};

struct Vertex {
    float3 pos;
    float3 normal;
};

struct Instance {
	float3 pos;
	float4 rot;
    float3 scale;

	float3 albedo_color;
	float roughness;
	float metallic;
	float specular;
	
	float3 wireframe_front_color;
	float4 wireframe_back_color;
	float3 wireframe_tess_front_color;
	float4 wireframe_tess_back_color;
	float wireframe_tess_split_count;
	float wireframe_tess_gap;
};

struct VertexOut {
	float4 pos : SV_POSITION;
	float3 world_pos : WORLD_POS;
	float3 vertex_normal : VERTEX_NORMAL;
	
	nointerpolation uint vertex_id : VERTEX_ID;
	nointerpolation uint primitive_id : PRIMITIVE_ID;
	nointerpolation uint instance_id : INSTANCE_ID;
	
	float tess_edge : TESS_EDGE;
	float tess_edge_dir : TESS_EDGE_DIRECTION;
};
