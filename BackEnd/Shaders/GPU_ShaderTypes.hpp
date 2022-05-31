#pragma once

// DirectX 12 Types
#include <DirectXMath.h>

// GLM
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtc/quaternion.hpp"

// Mine
#include <CommonTypes.hpp>


DirectX::XMFLOAT3 dxConvert(glm::vec3& value);
DirectX::XMFLOAT3 dxConvert(float x, float y, float z);
DirectX::XMFLOAT4 dxConvert(glm::vec4& value);
DirectX::XMFLOAT4 dxConvert(glm::quat& value);
DirectX::XMFLOAT4X4 dxConvert(glm::mat4& value);
DirectX::XMMATRIX dxConvertMatrix(glm::mat4& value);


struct GPU_Vertex {
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 normal;
};


struct GPU_MeshTriangle {
	DirectX::XMFLOAT3 poly_normal;
	DirectX::XMFLOAT3 tess_normal;
	uint32_t tess_vertex_0;
	uint32_t tess_vertex_1;
};


struct GPU_UnplacedVertexGroup {
	uint32_t vert_idxs[21];
};

struct GPU_PlacedVertexGroup {
	// vertex ,axis, level
	int32_t vert_idxs[21][3][5];
};

struct GPU_IndexUpdateGroup {
	u32 poly_idx[12];
	u32 new_vertex_idx[12][6];
};

struct GPU_VertexPositionUpdateGroup {
	uint32_t vertex_id[64];
	DirectX::XMFLOAT3 new_pos[64];
};

struct GPU_VertexNormalUpdateGroup {
	uint32_t vertex_id[64];
	DirectX::XMFLOAT3 new_normal[64];
};

// each poly is made of TWO tesselation triangles
struct GPU_PolyNormalUpdateGroup {
	uint32_t tess_idxs[32][2];  // idx of tess triangles to update
	uint32_t poly_verts[32][4];  // vertices of polygon
	uint32_t tess_type[32];  // how is the polygon split
	uint32_t tess_split_vertices[32][2];
};

struct GPU_Result_PolyNormalUpdateGroup {
	glm::vec3 poly_normal[32];
	glm::vec3 tess_normals[32][2];
};
