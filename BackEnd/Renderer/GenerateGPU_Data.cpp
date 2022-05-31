#include "./Renderer.hpp"


void Renderer::generateGPU_Data()
{
	for (scme::SculptMesh& sculpt_mesh : app.meshes) {

		if (sculpt_mesh.verts.size() == 0) {
			continue;
		}

		// Figure out what to update
		bool dirty_vertex_list = false;
		bool dirty_vertex_pos = false;
		bool dirty_index_buff = false;
		bool dirty_tess_tris = false;
		bool dirty_instance = false;
		bool dirty_debug = false;
		{
			if (sculpt_mesh.verts.size() != sculpt_mesh.gpu_verts.count() - 1) {
				dirty_vertex_list = true;
			}

			for (scme::ModifiedVertex& modified_v : sculpt_mesh.modified_verts) {

				switch (modified_v.state) {
				case scme::ModifiedVertexState::UPDATE: {
					dirty_vertex_pos = true;
					break;
				}
				case scme::ModifiedVertexState::DELETED: {
					dirty_vertex_list = true;
				}
				}
			}

			for (scme::ModifiedPoly& modified_p : sculpt_mesh.modified_polys) {

				switch (modified_p.state) {
				case scme::ModifiedPolyState::UPDATE: {
					dirty_index_buff = true;
					break;
				}
				case scme::ModifiedPolyState::DELETED: {
					dirty_index_buff = true;
				}
				}
			}

			for (auto& modified_instance : sculpt_mesh.modified_instances) {
				
				switch (modified_instance.state) {
				case scme::ModifiedInstanceType::UPDATE: {
					dirty_instance = true;
					break;
				}
				case scme::ModifiedInstanceType::DELETED: {
					dirty_instance = true;
				}
				}
			}

			if (dirty_vertex_list || dirty_vertex_pos ||
				dirty_index_buff || dirty_tess_tris ||
				dirty_instance)
			{
				dirty_debug = true;
			}
		}	

		// Update Mesh Data
		{
			if (dirty_debug) {
				Context::beginPixCapture();
			}

			if (dirty_vertex_list) {
				sculpt_mesh.uploadVertexAddsRemoves();
			}

			if (dirty_vertex_pos) {
				sculpt_mesh.uploadVertexPositions();
			}

			if (dirty_index_buff) {
				sculpt_mesh.uploadIndexBufferChanges();
			}

			if (dirty_tess_tris) {
				sculpt_mesh.uploadTesselationTriangles(scme::TesselationModificationBasis::MODIFIED_POLYS);
			}

			if (app.shading_normal == GPU_ShadingNormal::VERTEX && dirty_vertex_pos)
			{
				sculpt_mesh.uploadVertexNormals();
			}

			if (sculpt_mesh.modified_verts.size() > 0) {

				for (scme::ModifiedVertex& modified_v : sculpt_mesh.modified_verts) {

					switch (modified_v.state) {
					case scme::ModifiedVertexState::UPDATE: {

						// it is posible to have the vertex as both deleted and updated multiple times
						// when running multiple sculpt mesh operations
						if (sculpt_mesh.verts.isDeleted(modified_v.idx) == false) {

							//sculpt_mesh.moveVertexInAABBs(modified_v.idx);
						}
						break;
					}
					}
				}
			}

			if (dirty_instance) {
				sculpt_mesh.uploadInstances();
			}

			sculpt_mesh.modified_verts.clear();
			sculpt_mesh.modified_polys.clear();
			sculpt_mesh.modified_instances.clear();

			if (dirty_debug) {
				Context::endPixCapture();
			}
		}
	}
}