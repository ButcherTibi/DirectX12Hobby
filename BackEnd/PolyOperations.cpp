#include "SculptMesh.hpp"

#include "glm/geometric.hpp"

using namespace scme;


//glm::vec3 SculptMesh::calcNormalForTrisPositions(Vertex* v0, Vertex* v1, Vertex* v2)
//{
//	glm::vec3 dir_0 = glm::normalize(v1->pos - v0->pos);
//	glm::vec3 dir_1 = glm::normalize(v2->pos - v0->pos);
//
//	return glm::normalize(-glm::cross(dir_0, dir_1));
//}

glm::vec3 SculptMesh::calcWindingNormal(Vertex* v0, Vertex* v1, Vertex* v2)
{
	return -glm::normalize(glm::cross(v1->pos - v0->pos, v2->pos - v0->pos));
}

void SculptMesh::calcPolyNormal(Poly* poly)
{
	if (poly->is_tris) {

		std::array<scme::Vertex*, 3> vs;
		getTrisPrimitives(poly, vs);

		// Triangle Normal
		poly->normal = calcWindingNormal(vs[0], vs[1], vs[2]);
		poly->tess_normals[0] = poly->normal;
		poly->tess_normals[1] = poly->normal;
	}
	else {
		std::array<scme::Vertex*, 4> vs;
		getQuadPrimitives(poly, vs);

		// Tesselation and Normals
		if (glm::distance(vs[0]->pos, vs[2]->pos) < glm::distance(vs[1]->pos, vs[3]->pos)) {

			poly->tesselation_type = 0;
			poly->tess_normals[0] = calcWindingNormal(vs[0], vs[1], vs[2]);
			poly->tess_normals[1] = calcWindingNormal(vs[0], vs[2], vs[3]);
		}
		else {
			poly->tesselation_type = 1;
			poly->tess_normals[0] = calcWindingNormal(vs[0], vs[1], vs[3]);
			poly->tess_normals[1] = calcWindingNormal(vs[1], vs[2], vs[3]);
		}
		poly->normal = glm::normalize((poly->tess_normals[0] + poly->tess_normals[1]) / 2.f);
	}
}

void SculptMesh::deletePoly(PolyIndex delete_poly_idx)
{
	Poly* delete_poly = &polys[delete_poly_idx];

	uint32_t count;
	if (delete_poly->is_tris) {
		count = 3;
	}
	else {
		count = 4;
	}

	for (uint8_t i = 0; i < count; i++) {

		uint32_t edge_idx = delete_poly->edges[i];
		Edge* edge = &edges[edge_idx];

		Vertex* vertex = &verts[edge->v0];
		_unregisterEdgeFromVertexEdgeList(edge, edge->v0, vertex);

		if (edge->nextEdgeOf(edge->v0) == edge_idx) {
			_deleteVertexMemory(edge->v0);
		}

		vertex = &verts[edge->v1];
		_unregisterEdgeFromVertexEdgeList(edge, edge->v1, vertex);

		if (edge->nextEdgeOf(edge->v1) == edge_idx) {
			_deleteVertexMemory(edge->v1);
		}

		// unregister poly from edge
		// because edge is part of poly one poly reference will reference poly
		if (edge->p0 == delete_poly_idx) {
			edge->p0 = 0xFFFF'FFFF;
		}
		else {
			edge->p1 = 0xFFFF'FFFF;
		}

		// edge is wire
		if (edge->p0 == edge->p1) {
			_deleteEdgeMemory(edge_idx);
		}
	}

	// Now no edges reference the poly so we can safely delete the polygon
	_deletePolyMemory(delete_poly_idx);
}
