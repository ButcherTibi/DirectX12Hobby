#include "SculptMesh.hpp"

using namespace scme;


void SculptMesh::_deleteVertexMemory(uint32_t vertex_idx)
{
	verts.erase(vertex_idx);

	ModifiedVertex& modified_vertex = modified_verts.emplace_back();
	modified_vertex.idx = vertex_idx;
	modified_vertex.state = ModifiedVertexState::DELETED;
}

uint32_t SculptMesh::_createEdge(VertexIndex v0, VertexIndex v1)
{
	uint32_t new_loop_idx;;
	edges.emplace(new_loop_idx);

	_setEdge(new_loop_idx, v0, v1);
	return new_loop_idx;
}

void SculptMesh::_setEdge(EdgeIndex existing_edge_idx, VertexIndex v0_idx, VertexIndex v1_idx)
{
	Edge* existing_edge = &edges[existing_edge_idx];
	existing_edge->v0 = v0_idx;
	existing_edge->v1 = v1_idx;
	existing_edge->p0 = 0xFFFF'FFFF;
	existing_edge->p1 = 0xFFFF'FFFF;
	existing_edge->was_raycast_tested = false;

	_registerEdgeToVertexEdgeList(existing_edge_idx, v0_idx);
	_registerEdgeToVertexEdgeList(existing_edge_idx, v1_idx);
}

void SculptMesh::_deleteEdgeMemory(EdgeIndex edge_idx)
{
	edges.erase(edge_idx);
}

void SculptMesh::_registerEdgeToVertexEdgeList(EdgeIndex new_edge_idx, VertexIndex vertex_idx)
{
	Edge& new_edge = edges[new_edge_idx];
	Vertex& vertex = verts[vertex_idx];

	// if vertex is point then vertex loop list is unused
	if (vertex.edge == 0xFFFF'FFFF) {

		new_edge.setPrevNextEdges(vertex_idx, new_edge_idx, new_edge_idx);
		vertex.edge = new_edge_idx;
	}
	else {
		uint32_t prev_edge_idx = vertex.edge;
		Edge& prev_edge = edges[prev_edge_idx];

		uint32_t next_edge_idx = prev_edge.nextEdgeOf(vertex_idx);
		Edge& next_edge = edges[next_edge_idx];

		// prev <--- new ---> next
		new_edge.setPrevNextEdges(vertex_idx, prev_edge_idx, next_edge_idx);

		// prev ---> new <--- next
		prev_edge.nextEdgeOf(vertex_idx) = new_edge_idx;
		next_edge.prevEdgeOf(vertex_idx) = new_edge_idx;
	}
}

void SculptMesh::_unregisterEdgeFromVertexEdgeList(Edge* delete_edge, VertexIndex vertex_idx, Vertex* vertex)
{
	if (vertex->edge != 0xFFFF'FFFF) {

		uint32_t prev_edge_idx = delete_edge->prevEdgeOf(vertex_idx);
		uint32_t next_edge_idx = delete_edge->nextEdgeOf(vertex_idx);

		Edge& prev_edge = edges[prev_edge_idx];
		Edge& next_edge = edges[next_edge_idx];

		// prev <---> next
		prev_edge.nextEdgeOf(vertex_idx) = next_edge_idx;
		next_edge.prevEdgeOf(vertex_idx) = prev_edge_idx;

		// make sure that the edge list has proper start or else infinite looping can occur
		// because start_edge is not in edge list
		vertex->edge = next_edge_idx;
	}
}
