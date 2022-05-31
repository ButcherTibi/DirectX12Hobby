#pragma once


// Size and position of a instance
struct Transform {
	glm::vec3 pos = { .0f, .0f, .0f };
	glm::quat rot = { 1.0f, .0f, .0f, .0f };
	glm::vec3 scale = { 1.0f, 1.0f, 1.0f };
};

// The apperance of the material beloging to a mesh
struct PhysicalBasedMaterial {
	glm::vec3 albedo_color = { 1.0f, 0.0f, 0.0f };
	float roughness = 0.3f;
	float metallic = 0.1f;
	float specular = 0.04f;
};

// Coloring of a instance's wireframe
struct WireframeColors {
	glm::vec3 front_color = { 0.0f, 1.0f, 0.0f };
	glm::vec4 back_color = { 0.0f, 0.20f, 0.0f, 1.0f };
	glm::vec3 tesselation_front_color = { 0.0f, 1.0f, 0.0f };
	glm::vec4 tesselation_back_color = { 0.0f, 0.20f, 0.0f, 1.0f };
	float tesselation_split_count = 4.0f;
	float tesselation_gap = 0.5f;
};

struct GPU_Instance {
	// Transform
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT4 rot;
	DirectX::XMFLOAT3 scale;

	// Material
	DirectX::XMFLOAT3 albedo_color;
	float roughness;
	float metallic;
	float specular;

	// Wireframe
	DirectX::XMFLOAT3 wireframe_front_color;
	DirectX::XMFLOAT4 wireframe_back_color;
	DirectX::XMFLOAT3 wireframe_tess_front_color;
	DirectX::XMFLOAT4 wireframe_tess_back_color;
	float wireframe_tess_split_count;
	float wireframe_tess_gap;
};

struct GPU_InstanceUpdateGroup {
	u32 instance_idx[64];
	GPU_Instance new_data[64];
};
