#include "CommonTypes.hlsli"
#include "SpatialMath.hlsli"

#pragma warning( disable : 3578 )

ConstantBuffer<Frame> frame : register(b0);
StructuredBuffer<Vertex> verts : register(t0);
StructuredBuffer<Instance> instances : register(t1);

//RootSignature
VertexOut main(uint vertex_id : SV_VertexID, uint instance_id : SV_InstanceID)
{
    VertexOut output;
    Vertex vertex = verts.Load(vertex_id);
    Instance instance = instances.Load(instance_id);

    // Discard instance, polygon or vertex
	if (instance.rot.w == 2.f || vertex_id == 0) {
		output.pos.xyz = float3( 0, 0, 0 );
		return output;
	}

    // World Position
	float3 pos = vertex.pos;
	pos = quatRotate(pos, instance.rot);
	pos += instance.pos;

    // Camera Position
	pos -= frame.camera_pos;
	pos = quatRotate(pos, frame.camera_quat_inv);

    // Viewport Position
	float4 persp = mul(frame.perspective, float4(pos, 1.f));  // perspective transform
	
	// Fix Depth
	persp.z = getLerp(frame.z_near, persp.w, frame.z_far);
	persp.z *= persp.w;
	output.pos = persp;

    // Output
	output.vertex_normal = vertex.normal;
	
	output.vertex_id = vertex_id;
	output.primitive_id = 0xFFFFFFFF;
	output.instance_id = instance_id;
	
	output.tess_edge = 0.12345;
	output.tess_edge_dir = 0.12345;
    return output;
}
