#include "SculptMesh.hpp"

using namespace scme;


uint32_t SculptMesh::_createTris(VertexIndex v0, VertexIndex v1, VertexIndex v2)
{
	uint32_t new_tris_idx;
	polys.emplace(new_tris_idx);

	_setTris(new_tris_idx, v0, v1, v2);

	return new_tris_idx;
}

uint32_t SculptMesh::_createQuad(VertexIndex v0, VertexIndex v1, VertexIndex v2, VertexIndex v3)
{
	uint32_t new_quad_idx;
	polys.emplace(new_quad_idx);

	_setQuad(new_quad_idx, v0, v1, v2, v3);

	return new_quad_idx;
}

void SculptMesh::_stichVerticesToVertexLooped(std::vector<VertexIndex>& vertices, VertexIndex target)
{
	uint32_t last = (uint32_t)vertices.size() - 1;
	for (uint32_t i = 0; i < last; i++) {

		uint32_t v = vertices[i];
		uint32_t v_next = vertices[i + 1];

		_createTris(v, target, v_next);
	}

	_createTris(vertices[last], target, vertices[0]);
}

void SculptMesh::_setTris(PolyIndex new_poly_idx, VertexIndex v0, VertexIndex v1, VertexIndex v2)
{
	Poly& new_poly = polys[new_poly_idx];
	new_poly.is_tris = 1;
	new_poly.edges[0] = addEdge(v0, v1);
	new_poly.edges[1] = addEdge(v1, v2);
	new_poly.edges[2] = addEdge(v2, v0);

	Edge* edge = &edges[new_poly.edges[0]];
	new_poly.flip_edge_0 = edge->v0 != v0;

	edge = &edges[new_poly.edges[1]];
	new_poly.flip_edge_1 = edge->v0 != v1;

	edge = &edges[new_poly.edges[2]];
	new_poly.flip_edge_2 = edge->v0 != v2;

	_registerPolyToEdge(new_poly_idx, new_poly.edges[0]);
	_registerPolyToEdge(new_poly_idx, new_poly.edges[1]);
	_registerPolyToEdge(new_poly_idx, new_poly.edges[2]);

	markPolyFullUpdate(new_poly_idx);
}

void SculptMesh::_setQuad(PolyIndex new_quad_idx, VertexIndex v0, VertexIndex v1, VertexIndex v2, VertexIndex v3)
{
	Poly& new_quad = polys[new_quad_idx];
	new_quad.is_tris = false;
	new_quad.edges[0] = addEdge(v0, v1);
	new_quad.edges[1] = addEdge(v1, v2);
	new_quad.edges[2] = addEdge(v2, v3);
	new_quad.edges[3] = addEdge(v3, v0);

	Edge* edge = &edges[new_quad.edges[0]];
	new_quad.flip_edge_0 = edge->v0 != v0;

	edge = &edges[new_quad.edges[1]];
	new_quad.flip_edge_1 = edge->v0 != v1;

	edge = &edges[new_quad.edges[2]];
	new_quad.flip_edge_2 = edge->v0 != v2;

	edge = &edges[new_quad.edges[3]];
	new_quad.flip_edge_3 = edge->v0 != v3;

	_registerPolyToEdge(new_quad_idx, new_quad.edges[0]);
	_registerPolyToEdge(new_quad_idx, new_quad.edges[1]);
	_registerPolyToEdge(new_quad_idx, new_quad.edges[2]);
	_registerPolyToEdge(new_quad_idx, new_quad.edges[3]);

	markPolyFullUpdate(new_quad_idx);
}

void SculptMesh::_registerPolyToEdge(PolyIndex new_poly_idx, EdgeIndex edge_idx)
{
	Edge& edge = edges[edge_idx];
	if (edge.p0 == 0xFFFF'FFFF) {
		edge.p0 = new_poly_idx;
	}
	else {
		edge.p1 = new_poly_idx;
	}
}

void SculptMesh::_unregisterPolyFromEdge(PolyIndex delete_poly_idx, EdgeIndex edge_idx)
{
	Edge& edge = edges[edge_idx];
	if (edge.p0 == delete_poly_idx) {
		edge.p0 = 0xFFFF'FFFF;
	}
	else {
		edge.p1 = 0xFFFF'FFFF;
	}
}

void SculptMesh::_deletePolyMemory(PolyIndex poly_idx)
{
	polys.erase(poly_idx);

	ModifiedPoly& modified_poly = modified_polys.emplace_back();
	modified_poly.idx = poly_idx;
	modified_poly.state = ModifiedPolyState::DELETED;
}
