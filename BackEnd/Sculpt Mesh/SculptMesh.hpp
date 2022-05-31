#pragma once

// Standard
#include <array>

// GLM
#include "glm/vec3.hpp"

// Toolbox
#include "ButchersToolbox/SparseVector.hpp"

// Mine
#include <Math/Geometry.hpp>
#include <SimpleX12/Resource/Resource.hpp>
#include <Shaders/GPU_ShaderTypes.hpp>
#include <SimpleX12/Drawcall/Drawcall.hpp>
#include "./InstanceTypes.hpp"


// Needs:
// uint32_t edge_idx;
// Edge* edge;
#define iterEdgesAroundVertexStart \
	do { 

// edges around a vertex don't have consitent orientation because they are shared by different polygons
// so check at what end of the edge to iterate around
// so that you always iterate around the same vertex
#define iterEdgesAroundVertexEnd(vertex_idx, start_edge_idx) \
		edge_idx = edge->nextEdgeOf(vertex_idx); \
		edge = &edges[edge_idx]; \
	} while (edge_idx != start_edge_idx);


namespace scme {

	typedef uint32_t VertexIndex;
	typedef uint32_t EdgeIndex;
	typedef uint32_t PolyIndex;
	typedef u32 InstanceIndex;


	enum class ModifiedVertexState {
		UPDATE,
		DELETED
	};

	struct ModifiedVertex {
		/// <summary>
		/// Vertex that is changed and must be updated on the GPU
		/// </summary>
		uint32_t idx;

		ModifiedVertexState state;
	};

	struct Vertex {
	public:
		/// <summary>
		/// Position in mesh local space
		/// </summary>
		glm::vec3 pos;

		/// <summary>
		/// Vertex normal computed from connected polys
		/// </summary>
		glm::vec3 normal;

		/// <summary>
		/// Any edge connected to the vertex
		/// </summary>
		/// <remarks>
		/// If 0xFFFF'FFFF then no edge is connected and is point
		/// </remarks>
		uint32_t edge;

		// aabb == 0xFFFF'FFFF, vertex does not belong to any AABB
		uint32_t aabb;  // to leaf AABB does this vertex belong
		uint32_t idx_in_aabb;  // where to find vertex in parent AABB (makes AABB transfers faster)

	public:
		Vertex() {};  // suppress initialization

		void init();

		/// <summary>
		/// Checks if the vertex is connected to anything
		/// </summary>
		/// <returns>True if the vertex is NOT connected</returns>
		bool isPoint();
	};


	// NOTES:
	// - Making the AABB actully contain the vertices is a terrible idea if you ever
	//   need to update the mesh because you need to update all primitives that reference that vertex
	// - All AABBs get divided into 8, even if child AABBs are unused,
	//   this is to not require storing which vertices belong to each child in a buffer before assigning them to
	//   to the child AABB, as well as to enable resizing the AABB vector only once for all 8 children
	struct VertexBoundingBox {
		uint32_t parent;

		/// <summary>
		/// If it is a leaf then children[0] == 0xFFFF'FFFF
		/// </summary>
		uint32_t children[8];

		AxisBoundingBox3D<> aabb;
		glm::vec3 mid;

		uint32_t verts_deleted_count;  // how many empty slots does this AABB have
		std::vector<uint32_t> verts;  // indexes of contained vertices

		//bool _debug_show_tesselation;  // TODO:

	public:
		/// <summary>
		/// Checks if this AABB does have any children
		/// </summary>
		bool isLeaf();

		/// <summary>
		/// Checks if the AABB has vertices assigned to it
		/// </summary>
		bool hasVertices();

		/// <summary>
		/// Checks in which AABB does position reside, position must be inside AABB
		/// </summary>
		/// <param name="pos"></param>
		/// <returns>AABB index of the child AABB</returns>
		uint32_t inWhichChildDoesPositionReside(glm::vec3& pos);
	};


	/// <summary>
	/// Winged-edge data structure
	/// </summary>
	struct Edge {
		// Double Linked list of edges around vertices
		uint32_t v0;
		uint32_t v0_next_edge;  // next/prev edge around vertex 0
		uint32_t v0_prev_edge;

		uint32_t v1;
		uint32_t v1_next_edge;  // next/prev edge around vertex 1
		uint32_t v1_prev_edge;

		// Polygons
		uint32_t p0;
		uint32_t p1;

		uint8_t was_raycast_tested : 1,  // used in poly raycasting to mark edge as tested
			: 7;

		Edge() {};  // suppress initialization

		/// <summary>
		/// Edges don't have a consistent orientation so a list of edges around a vertex
		/// will not have ALL edges v0 == (vertex around to iterate)
		/// </summary>
		/// <param name="vertex">Vertex to iterate around</param>
		/// <returns>Next edge around vertex</returns>
		uint32_t& nextEdgeOf(uint32_t vertex);
		uint32_t& prevEdgeOf(uint32_t vertex);

		void setPrevNextEdges(uint32_t vertex, uint32_t prev_edge, uint32_t next_edge);
	};


	enum class ModifiedPolyState {
		UPDATE,
		DELETED
	};

	struct ModifiedPoly {
		uint32_t idx;
		ModifiedPolyState state;
	};

	struct Poly {
	public:
		// the normal of the triangle or 
		// the average normal of the 2 triangles composing the quad
		glm::vec3 normal;
		glm::vec3 tess_normals[2];

		uint32_t edges[4];

		// NOTE TO SELF: wrong bit field syntax breaks MSVC hard
		uint8_t
			/// <summary>
			/// split from 0 to 2 or from 1 to 3
			/// </summary>
			tesselation_type : 1,

			/// <summary>
			/// is it a triangle or quad
			/// </summary>
			is_tris : 1,

			/// <summary>
			/// Orientation of the edges for faster iteration of vertices around poly,
			/// do not replace with edges[i].v0 == edges[i + 1].v0 stuff, it's slower.
			/// </summary>
			flip_edge_0 : 1,
			flip_edge_1 : 1,
			flip_edge_2 : 1,
			flip_edge_3 : 1,
			: 2;


		Poly() {};  // suppress initialization
	};

	enum class TesselationModificationBasis {
		MODIFIED_POLYS,  // update the tesselation for each polygon

		// update the tesselation for each polygon around a modified vertex
		// this is slower and is used only when vertices are modified
		MODIFIED_VERTICES
	};


	enum class ModifiedInstanceType {
		UPDATE,
		DELETED
	};

	// What instance was modified and what was modified
	struct ModifiedInstance {
		uint32_t idx;

		ModifiedInstanceType state;
	};

	struct Instance {
		Transform transform;
		PhysicalBasedMaterial material;
		WireframeColors wireframe_colors;

		Instance() {};  // suppress initialization
	};


	class SculptMesh {
	public:
		// Vertex
		SparseVector<Vertex> verts;
		std::vector<ModifiedVertex> modified_verts;
		StorageBuffer<GPU_Vertex> gpu_verts;

		// Edge
		SparseVector<Edge> edges;

		// Poly
		SparseVector<Poly> polys;
		std::vector<ModifiedPoly> modified_polys;
		StorageBuffer<GPU_MeshTriangle> gpu_triangles;

		// Instances
		SparseVector<Instance> instances;
		std::vector<ModifiedInstance> modified_instances;
		StorageBuffer<GPU_Instance> gpu_instances;
		// ComPtr<ID3D11ShaderResourceView> gpu_instances_srv;

		// Settings
		uint32_t max_vertices_in_AABB;

		// Vertex Updates
		std::vector<GPU_VertexPositionUpdateGroup> vert_pos_updates;  // remove this and just use the GPU mapped memory
		StorageBuffer<GPU_VertexPositionUpdateGroup> gpu_vert_pos_updates;

		// Poly Updates
		StorageBuffer<GPU_IndexUpdateGroup> gpu_index_updates;
		IndexBuffer gpu_indexes;

		// Instance Updates
		StorageBuffer<GPU_InstanceUpdateGroup> gpu_instance_updates;

	private:
		void _init();


		/* Low Level ****************************************************/
		// These methods may not leave the mesh in a coherent state

		void _deleteVertexMemory(VertexIndex vertex_idx);
		void _deleteEdgeMemory(EdgeIndex edge_idx);
		void _deletePolyMemory(PolyIndex poly_idx);

		/// <summary>
		/// Register/Unregister a edge to the edge list around a vertex
		/// </summary>
		/// <param name="new_poly_idx"></param>
		/// <param name="edge_idx"></param>
		void _registerEdgeToVertexEdgeList(EdgeIndex new_edge_idx, VertexIndex vertex_idx);
		void _unregisterEdgeFromVertexEdgeList(Edge* delete_edge, uint32_t vertex_idx, Vertex* vertex);

		void _registerPolyToEdge(PolyIndex new_poly_idx, EdgeIndex edge_idx);
		void _unregisterPolyFromEdge(PolyIndex delete_poly_idx, EdgeIndex edge_idx);

		/// <summary>
		/// Always creates an edge between vertices
		/// </summary>
		/// <returns>Newly created edge</returns>
		uint32_t _createEdge(VertexIndex v0, VertexIndex v1);

		/// <summary>
		/// Initialize an existing edge and registers it to connected edges
		/// </summary>
		void _setEdge(EdgeIndex existing_edge_idx, VertexIndex v0_idx, VertexIndex v1_idx);

		/// <summary>
		/// Always adds a polygon
		/// </summary>
		uint32_t _createTris(VertexIndex v0, VertexIndex v1, VertexIndex v2);
		uint32_t _createQuad(VertexIndex v0, VertexIndex v1, VertexIndex v2, VertexIndex v3);

		void _stichVerticesToVertexLooped(std::vector<VertexIndex>& vertices, VertexIndex target);

		/// <summary>
		/// Initialize an existing poly and registers it to connected edges
		/// </summary>
		void _setTris(PolyIndex new_poly_idx, VertexIndex v0, VertexIndex v1, VertexIndex v2);
		void _setQuad(PolyIndex new_quad_idx, VertexIndex v0, VertexIndex v1, VertexIndex v2, VertexIndex v3);

	public:
		/* Creation *****************************************************/

		void createAsTriangle(float size);


		/* Vertex *******************************************************/

		/// <summary>
		/// Calculate vertex normal from polygon normals
		/// </summary>
		void calcVertexNormal(VertexIndex vertex_idx);

		// addVertex

		// moveVertex

		// @TODO:
		void deleteVertex(uint32_t);


		/* Edge *********************************************************/

		/// <summary>
		/// Ensures that a edge exists connected to the vertices
		/// </summary>
		/// <returns>Index to newly created edge, or to found existing edge</returns>
		uint32_t addEdge(VertexIndex v0, VertexIndex v1);

		/// <summary>
		/// Find edge connected to these two vertices 
		/// </summary>
		uint32_t findEdgeBetween(VertexIndex v0_idx, VertexIndex v1_idx);


		/* Poly *********************************************************/

		glm::vec3 calcWindingNormal(Vertex* v0, Vertex* v1, Vertex* v2);

		void calcPolyNormal(Poly* poly);

		void getTrisPrimitives(Poly* poly,
			std::array<uint32_t, 3>& r_vs_idxs, std::array<Vertex*, 3>& r_vs);
		void getTrisPrimitives(Poly* poly, std::array<VertexIndex, 3>& r_vs_idxs);
		void getTrisPrimitives(Poly* poly, std::array<Vertex*, 3>& r_vs);

		void getQuadPrimitives(Poly* poly,
			std::array<uint32_t, 4>& r_vs_idxs, std::array<Vertex*, 4>& r_vs);
		void getQuadPrimitives(Poly* poly, std::array<VertexIndex, 4>& r_vs_idxs);
		void getQuadPrimitives(Poly* poly, std::array<Vertex*, 4>& r_vs);

		// @TODO: is this even corect ?
		// just delete every edge that only references polygon
		// just delete every vertex that only refernces above edges
		void deletePoly(PolyIndex delete_poly_idx);


		/* Instance *****************************************************/

		InstanceIndex addInstance();


		/* Updates ******************************************************/

		// schedule a vertex to have it's data updated on the GPU side
		void markVertexFullUpdate(uint32_t vertex);

		void markVertexMoved(uint32_t vertex);

		// schedule a poly to have it's data updated on the GPU side
		void markPolyFullUpdate(uint32_t poly);

		// used changing the mesh shading mode
		void markAllVerticesForNormalUpdate();

		void markInstanceUpdate(InstanceIndex instance);

		// upload vertex additions and removals to GPU
		void uploadVertexAddsRemoves();

		// upload vertex positions changes to GPU
		void uploadVertexPositions();

		// upload vertex normals changes to GPU
		void uploadVertexNormals();

		// upload poly additions and removals to GPU
		void uploadIndexBufferChanges();

		// uploads which tesselation triangles have changed
		// computes on the GPU normals for polygons
		// downloads the results and applies them
		// 
		// the based_on parameter is used to determine on what basis should the tesselation be updated
		void uploadTesselationTriangles(
			TesselationModificationBasis based_on = TesselationModificationBasis::MODIFIED_POLYS);

		void uploadInstances();


		/* Debug ********************************************************/

		void printVerices();
		void printEdgeListOfVertex(VertexIndex vertex_idx);
	};
}
