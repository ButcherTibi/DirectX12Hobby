#include "SculptMesh.hpp"


using namespace scme;


void Vertex::init()
{
	edge = 0xFFFF'FFFF;
	aabb = 0xFFFF'FFFF;
}

bool Vertex::isPoint()
{
	return edge == 0xFFFF'FFFF;
}

bool VertexBoundingBox::isLeaf()
{
	return children[0] == 0xFFFF'FFFF;
}

bool VertexBoundingBox::hasVertices()
{
	return verts.size() - verts_deleted_count > 0;
}

uint32_t VertexBoundingBox::inWhichChildDoesPositionReside(glm::vec3& pos)
{
	assert_cond(isLeaf() == false, "should not be called for leafs because leafs don't have children");

	// above
	if (pos.y >= mid.y) {

		// forward
		if (pos.z >= mid.z) {

			// left
			if (pos.x <= mid.x) {
				return children[0];
			}
			// right
			else {
				return children[1];
			}
		}
		// back
		else {
			// left
			if (pos.x <= mid.x) {
				return children[2];
			}
			// right
			else {
				return children[3];
			}
		}
	}
	// below
	else {
		// forward
		if (pos.z >= mid.z) {

			// left
			if (pos.x <= mid.x) {
				return children[4];
			}
			// right
			else {
				return children[5];
			}
		}
		// back
		else {
			// left
			if (pos.x <= mid.x) {
				return children[6];
			}
		}
	}

	// right
	return children[7];
}

uint32_t& Edge::nextEdgeOf(uint32_t vertex_idx)
{
	if (v0 == vertex_idx) {
		return v0_next_edge;
	}

	assert_cond(v1 == vertex_idx, "malformed iteration of edge around vertex");

	return v1_next_edge;
}

uint32_t& Edge::prevEdgeOf(uint32_t vertex_idx)
{
	if (v0 == vertex_idx) {
		return v0_prev_edge;
	}

	assert_cond(v1 == vertex_idx, "malformed iteration of edge around vertex");

	return v1_prev_edge;
}

void Edge::setPrevNextEdges(uint32_t vertex_idx, uint32_t prev_edge_idx, uint32_t next_edge_idx)
{
	if (v0 == vertex_idx) {
		v0_prev_edge = prev_edge_idx;
		v0_next_edge = next_edge_idx;
	}
	else {
		v1_prev_edge = prev_edge_idx;
		v1_next_edge = next_edge_idx;
	}
}

void SculptMesh::getTrisPrimitives(Poly* poly,
	std::array<uint32_t, 3>& r_vs_idxs, std::array<Vertex*, 3>& r_vs)
{
	std::array<Edge*, 3> r_es;
	r_es[0] = &edges[poly->edges[0]];
	r_es[1] = &edges[poly->edges[1]];
	r_es[2] = &edges[poly->edges[2]];

	r_vs_idxs[0] = poly->flip_edge_0 ? r_es[0]->v1 : r_es[0]->v0;
	r_vs_idxs[1] = poly->flip_edge_1 ? r_es[1]->v1 : r_es[1]->v0;
	r_vs_idxs[2] = poly->flip_edge_2 ? r_es[2]->v1 : r_es[2]->v0;

	r_vs[0] = &verts[r_vs_idxs[0]];
	r_vs[1] = &verts[r_vs_idxs[1]];
	r_vs[2] = &verts[r_vs_idxs[2]];
}

void SculptMesh::getTrisPrimitives(Poly* poly, std::array<uint32_t, 3>& r_vs_idxs)
{
	std::array<Edge*, 3> r_es;
	r_es[0] = &edges[poly->edges[0]];
	r_es[1] = &edges[poly->edges[1]];
	r_es[2] = &edges[poly->edges[2]];

	r_vs_idxs[0] = poly->flip_edge_0 ? r_es[0]->v1 : r_es[0]->v0;
	r_vs_idxs[1] = poly->flip_edge_1 ? r_es[1]->v1 : r_es[1]->v0;
	r_vs_idxs[2] = poly->flip_edge_2 ? r_es[2]->v1 : r_es[2]->v0;
}

void SculptMesh::getTrisPrimitives(Poly* poly, std::array<Vertex*, 3>& r_vs)
{
	std::array<Edge*, 3> r_es;
	r_es[0] = &edges[poly->edges[0]];
	r_es[1] = &edges[poly->edges[1]];
	r_es[2] = &edges[poly->edges[2]];

	std::array<uint32_t, 3> r_vs_idxs;
	r_vs_idxs[0] = poly->flip_edge_0 ? r_es[0]->v1 : r_es[0]->v0;
	r_vs_idxs[1] = poly->flip_edge_1 ? r_es[1]->v1 : r_es[1]->v0;
	r_vs_idxs[2] = poly->flip_edge_2 ? r_es[2]->v1 : r_es[2]->v0;

	r_vs[0] = &verts[r_vs_idxs[0]];
	r_vs[1] = &verts[r_vs_idxs[1]];
	r_vs[2] = &verts[r_vs_idxs[2]];
}

void SculptMesh::getQuadPrimitives(Poly* poly,
	std::array<uint32_t, 4>& r_vs_idxs, std::array<Vertex*, 4>& r_vs)
{
	std::array<Edge*, 4> r_es;
	r_es[0] = &edges[poly->edges[0]];
	r_es[1] = &edges[poly->edges[1]];
	r_es[2] = &edges[poly->edges[2]];
	r_es[3] = &edges[poly->edges[3]];

	r_vs_idxs[0] = poly->flip_edge_0 ? r_es[0]->v1 : r_es[0]->v0;
	r_vs_idxs[1] = poly->flip_edge_1 ? r_es[1]->v1 : r_es[1]->v0;
	r_vs_idxs[2] = poly->flip_edge_2 ? r_es[2]->v1 : r_es[2]->v0;
	r_vs_idxs[3] = poly->flip_edge_3 ? r_es[3]->v1 : r_es[3]->v0;

	r_vs[0] = &verts[r_vs_idxs[0]];
	r_vs[1] = &verts[r_vs_idxs[1]];
	r_vs[2] = &verts[r_vs_idxs[2]];
	r_vs[3] = &verts[r_vs_idxs[3]];
}

void SculptMesh::getQuadPrimitives(Poly* poly, std::array<uint32_t, 4>& r_vs_idxs)
{
	std::array<Edge*, 4> r_es;
	r_es[0] = &edges[poly->edges[0]];
	r_es[1] = &edges[poly->edges[1]];
	r_es[2] = &edges[poly->edges[2]];
	r_es[3] = &edges[poly->edges[3]];

	r_vs_idxs[0] = poly->flip_edge_0 ? r_es[0]->v1 : r_es[0]->v0;
	r_vs_idxs[1] = poly->flip_edge_1 ? r_es[1]->v1 : r_es[1]->v0;
	r_vs_idxs[2] = poly->flip_edge_2 ? r_es[2]->v1 : r_es[2]->v0;
	r_vs_idxs[3] = poly->flip_edge_3 ? r_es[3]->v1 : r_es[3]->v0;
}

void SculptMesh::getQuadPrimitives(Poly* poly, std::array<Vertex*, 4>& r_vs)
{
	std::array<Edge*, 4> r_es;
	r_es[0] = &edges[poly->edges[0]];
	r_es[1] = &edges[poly->edges[1]];
	r_es[2] = &edges[poly->edges[2]];
	r_es[3] = &edges[poly->edges[3]];

	std::array<uint32_t, 4> r_vs_idxs;
	r_vs_idxs[0] = poly->flip_edge_0 ? r_es[0]->v1 : r_es[0]->v0;
	r_vs_idxs[1] = poly->flip_edge_1 ? r_es[1]->v1 : r_es[1]->v0;
	r_vs_idxs[2] = poly->flip_edge_2 ? r_es[2]->v1 : r_es[2]->v0;
	r_vs_idxs[3] = poly->flip_edge_3 ? r_es[3]->v1 : r_es[3]->v0;

	r_vs[0] = &verts[r_vs_idxs[0]];
	r_vs[1] = &verts[r_vs_idxs[1]];
	r_vs[2] = &verts[r_vs_idxs[2]];
	r_vs[3] = &verts[r_vs_idxs[3]];
}

void SculptMesh::printVerices()
{
	for (auto iter = verts.begin(); iter != verts.after(); iter.next()) {

		Vertex& vertex = iter.get();

		printf("vertex[%d].pos = { %.2f, %.2f %.2f } \n",
			iter.index(),
			vertex.pos.x,
			vertex.pos.y,
			vertex.pos.z
		);
	}
}
