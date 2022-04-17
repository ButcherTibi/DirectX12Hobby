#include "SculptMesh.hpp"

using namespace scme;


void SculptMesh::markVertexFullUpdate(VertexIndex vertex)
{
	ModifiedVertex& modified_vertex = modified_verts.emplace_back();
	modified_vertex.idx = vertex;
	modified_vertex.state = ModifiedVertexState::UPDATE;
}

void SculptMesh::markVertexMoved(VertexIndex vertex)
{
	ModifiedVertex& modified_vertex = modified_verts.emplace_back();
	modified_vertex.idx = vertex;
	modified_vertex.state = ModifiedVertexState::UPDATE;

	this->dirty_vertex_pos = true;
	this->dirty_vertex_normals = true;
	this->dirty_tess_tris = true;
}

void SculptMesh::markPolyFullUpdate(uint32_t poly)
{
	ModifiedPoly& modified_poly = modified_polys.emplace_back();
	modified_poly.idx = poly;
	modified_poly.state = ModifiedPolyState::UPDATE;
}
