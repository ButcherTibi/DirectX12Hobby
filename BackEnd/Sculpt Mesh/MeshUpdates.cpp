#include "./SculptMesh.hpp"

#include <Renderer/Renderer.hpp>


using namespace scme;

void SculptMesh::markAllVerticesForNormalUpdate()
{
	modified_verts.resize(verts.size());

	uint32_t i = 0;
	for (auto iter = verts.begin(); iter != verts.after(); iter.next()) {

		ModifiedVertex& modified_vertex = modified_verts[i];
		modified_vertex.idx = iter.index();
		modified_vertex.state = ModifiedVertexState::UPDATE;

		i++;
	}
}

void SculptMesh::uploadVertexAddsRemoves()
{
	if (gpu_verts.count() < verts.capacity() + 1) {
		gpu_verts.resize(verts.capacity() + 1);
	}

	//gpu_verts.mapNoRead();

	//for (scme::ModifiedVertex& modified_v : modified_verts) {

	//	// remove vertices
	//	if (modified_v.state == ModifiedVertexState::DELETED) {

	//		GPU_Vertex gpu_v;
	//		gpu_v.normal.x = 999'999.f;

	//		gpu_verts.update(modified_v.idx + 1, gpu_v);
	//		break;
	//	}
	//}

	//gpu_verts.unmap();
}

void SculptMesh::uploadVertexPositions()
{
	// Counting
	{
		uint32_t group_count = 1;
		uint32_t thread_count = 0;

		for (scme::ModifiedVertex& modified_v : modified_verts) {

			if (modified_v.state == ModifiedVertexState::UPDATE) {

				thread_count++;

				if (thread_count == 64) {
					group_count++;
					thread_count = 0;
				}
			}
		}

		vert_pos_updates.resize(group_count);
	}

	uint32_t group_idx = 0;
	uint32_t thread_idx = 0;

	for (scme::ModifiedVertex& modified_v : modified_verts) {

		if (modified_v.state == ModifiedVertexState::UPDATE) {

			auto& update = vert_pos_updates[group_idx];
			update.vertex_id[thread_idx] = modified_v.idx + 1;
			update.new_pos[thread_idx] = dxConvert(verts[modified_v.idx].pos);

			thread_idx++;

			if (thread_idx == 64) {
				group_idx++;
				thread_idx = 0;
			}
		}
	}

	// Round Down Threads
	uint32_t last_thread = thread_idx;
	for (thread_idx = last_thread; thread_idx < 64; thread_idx++) {
		vert_pos_updates[group_idx].vertex_id[thread_idx] = 0;
	}

	// Load
	gpu_vert_pos_updates.upload(vert_pos_updates);

	// Command List
	auto& r = renderer;
	auto& ctx = r.context;
	{
		ctx.beginCommandList();

		r.vert_pos_update_call.CMD_bind();
		r.vert_pos_update_call.CMD_setBufferParam(0, gpu_vert_pos_updates);
		r.vert_pos_update_call.CMD_setUnorderedAccessResourceParam(0, gpu_verts);
		r.vert_pos_update_call.CMD_dispatch(
			(u32)vert_pos_updates.size(), 1, 1);

		ctx.endAndWaitForCommandList();
	}
}

void SculptMesh::uploadVertexNormals()
{
	//assert_cond(dirty_tess_tris == false);

	//if (modified_verts.size() > 0) {

	//	auto& r = renderer;

	//	// Counting
	//	{
	//		uint32_t group_count = 1;
	//		uint32_t thread_count = 0;

	//		for (scme::ModifiedVertex& modified_v : modified_verts) {

	//			if (modified_v.state == ModifiedVertexState::UPDATE &&
	//				verts.isDeleted(modified_v.idx) == false)
	//			{
	//				thread_count++;

	//				if (thread_count == 64) {
	//					group_count++;
	//					thread_count = 0;
	//				}
	//			}
	//		}

	//		r.vert_normal_updates.resize(group_count);
	//	}

	//	uint32_t group_idx = 0;
	//	uint32_t thread_idx = 0;

	//	for (scme::ModifiedVertex& modified_v : modified_verts) {

	//		if (modified_v.state == ModifiedVertexState::UPDATE &&
	//			verts.isDeleted(modified_v.idx) == false)
	//		{
	//			calcVertexNormal(modified_v.idx);

	//			// Load
	//			auto& update = r.vert_normal_updates[group_idx];
	//			update.vertex_id[thread_idx] = modified_v.idx + 1;
	//			update.new_normal[thread_idx] = dxConvert(verts[modified_v.idx].normal);

	//			// Increment
	//			thread_idx++;

	//			if (thread_idx == 64) {
	//				group_idx++;
	//				thread_idx = 0;
	//			}
	//		}
	//	}

	//	// Round Down Threads
	//	uint32_t last_thread = thread_idx;
	//	for (thread_idx = last_thread; thread_idx < 64; thread_idx++) {
	//		r.vert_normal_updates[group_idx].vertex_id[thread_idx] = 0;
	//	}

	//	// Load
	//	r.gpu_vert_normal_updates.upload(r.vert_normal_updates);

	//	// Command List
	//	auto& ctx = r.im_ctx3;
	//	ctx->ClearState();

	//	// Shader Resources
	//	{
	//		std::array<ID3D11ShaderResourceView*, 1> srvs = {
	//			r.gpu_vert_normal_updates.getSRV()
	//		};
	//		ctx->CSSetShaderResources(0, srvs.size(), srvs.data());
	//	}

	//	// UAVs
	//	{
	//		std::array<ID3D11UnorderedAccessView*, 1> uavs = {
	//			gpu_verts.getUAV()
	//		};
	//		ctx->CSSetUnorderedAccessViews(0, uavs.size(), uavs.data(), nullptr);
	//	}

	//	ctx->CSSetShader(r.update_vertex_normals_cs.Get(), nullptr, 0);

	//	ctx->Dispatch(r.vert_normal_updates.size(), 1, 1);
	//}
}

void SculptMesh::uploadIndexBufferChanges()
{
	const u32 thread_x_count = 12;

	if (gpu_indexes.count() < polys.capacity() * 6) {
		gpu_indexes.resize(polys.capacity() * 6);
	}

	// Counting
	{
		uint32_t group_count = 0;
		uint32_t thread_count = 0;

		for (u32 i = 0; i < modified_polys.size(); i++) {

			thread_count++;

			if (thread_count == thread_x_count) {
				group_count++;
				thread_count = 0;
			}
		}

		if (group_count == 0) {
			group_count += 1;
		}

		gpu_index_updates.resize(group_count);
		gpu_index_updates.mapNoRead();
	}

	// Load Data
	{
		uint32_t group_idx = 0;
		uint32_t thread_idx = 0;

		std::array<uint32_t, 3> vs3_idx;
		std::array<uint32_t, 4> vs4_idx;

		GPU_IndexUpdateGroup group = {};

		for (scme::ModifiedPoly& modified_poly : modified_polys) {

			switch (modified_poly.state) {
			case scme::ModifiedPolyState::UPDATE: {

				scme::Poly& poly = polys[modified_poly.idx];
				group.poly_idx[thread_idx] = modified_poly.idx + 1;

				if (poly.is_tris) {

					getTrisPrimitives(&poly, vs3_idx);

					group.new_vertex_idx[thread_idx][0] = vs3_idx[0];
					group.new_vertex_idx[thread_idx][1] = vs3_idx[1];
					group.new_vertex_idx[thread_idx][2] = vs3_idx[2];

					group.new_vertex_idx[thread_idx][3] = 0;
					group.new_vertex_idx[thread_idx][4] = 0;
					group.new_vertex_idx[thread_idx][5] = 0;
				}
				else {
					getQuadPrimitives(&poly, vs4_idx);

					if (poly.tesselation_type == 0) {

						group.new_vertex_idx[thread_idx][0] = vs3_idx[0];
						group.new_vertex_idx[thread_idx][1] = vs3_idx[2];
						group.new_vertex_idx[thread_idx][2] = vs3_idx[3];

						group.new_vertex_idx[thread_idx][3] = vs3_idx[0];
						group.new_vertex_idx[thread_idx][4] = vs3_idx[1];
						group.new_vertex_idx[thread_idx][5] = vs3_idx[2];
					}
					else {
						group.new_vertex_idx[thread_idx][0] = vs3_idx[0];
						group.new_vertex_idx[thread_idx][1] = vs3_idx[1];
						group.new_vertex_idx[thread_idx][2] = vs3_idx[3];

						group.new_vertex_idx[thread_idx][3] = vs3_idx[1];
						group.new_vertex_idx[thread_idx][4] = vs3_idx[2];
						group.new_vertex_idx[thread_idx][5] = vs3_idx[3];
					}
				}
				break;
			}

			case scme::ModifiedPolyState::DELETED: {

				for (u32 i = 0; i < 6; i++) {
					group.new_vertex_idx[thread_idx][i] = 0;
				}
				break;
			}
			}

			thread_idx++;

			if (thread_idx == thread_x_count) {

				gpu_index_updates.update(group_idx, group);
				group = {};

				group_idx++;
				thread_idx = 0;
			}
		}	

		if (group_idx == 0) {
			gpu_index_updates.update(0, group);
		}

		gpu_index_updates.unmap();
	}

	// Command List
	auto& r = renderer;
	auto& ctx = r.context;
	{
		ctx.beginCommandList();

		r.index_update_call.CMD_bind();
		r.index_update_call.CMD_setBufferParam(0, gpu_index_updates);
		r.index_update_call.CMD_setUnorderedAccessResourceParam(0, gpu_indexes);
		r.index_update_call.CMD_dispatch(
			(u32)gpu_index_updates.count(), 1, 1);

		ctx.endAndWaitForCommandList();
	}
}

void SculptMesh::uploadTesselationTriangles(TesselationModificationBasis)
{
//	assert_cond(dirty_vertex_pos == false);
//
//	if (modified_verts.size() > 0) {
//
//		auto& r = renderer;
//		
//		// regardless if a poly is tris or quad, always load 2 triangles
//		gpu_triangles.resize(polys.capacity() * 2);
//
//		r.poly_normal_updates.clear();
//
//		GPU_PolyNormalUpdateGroup* update = &r.poly_normal_updates.emplace_back();
//		uint32_t thread_idx = 0;
//
//		auto increment = [&]() {
//
//			thread_idx++;
//
//			if (thread_idx == 32) {
//				update = &r.poly_normal_updates.emplace_back();
//				thread_idx = 0;
//			}
//		};
//
//		auto group_updates = [&](uint32_t poly_idx) {
//
//			if (poly_idx != 0xFFFF'FFFF) {
//
//				Poly* poly = &polys[poly_idx];
//
//				if (poly->is_tris) {
//
//					std::array<uint32_t, 3> verts_idxes;
//					getTrisPrimitives(poly, verts_idxes);
//
//					for (uint32_t i = 0; i < 3; i++) {
//						verts_idxes[i] += 1;
//					}
//
//					update->tess_idxs[thread_idx][0] = poly_idx * 2;
//					update->tess_idxs[thread_idx][1] = 0xFFFFFFFF;
//					update->poly_verts[thread_idx][0] = verts_idxes[0];
//					update->poly_verts[thread_idx][1] = verts_idxes[1];
//					update->poly_verts[thread_idx][2] = verts_idxes[2];
//				}
//				else {
//					std::array<uint32_t, 4> verts_idxes;
//					getQuadPrimitives(poly, verts_idxes);
//
//					for (uint32_t i = 0; i < 4; i++) {
//						verts_idxes[i] += 1;
//					}
//
//					update->tess_idxs[thread_idx][0] = poly_idx * 2;
//					update->tess_idxs[thread_idx][1] = poly_idx * 2 + 1;
//					update->poly_verts[thread_idx][0] = verts_idxes[0];
//					update->poly_verts[thread_idx][1] = verts_idxes[1];
//					update->poly_verts[thread_idx][2] = verts_idxes[2];
//					update->poly_verts[thread_idx][3] = verts_idxes[3];
//
//					update->tess_type[thread_idx] = poly->tesselation_type;
//
//					if (poly->tesselation_type == 0) {
//						update->tess_split_vertices[thread_idx][0] = verts_idxes[0];
//						update->tess_split_vertices[thread_idx][1] = verts_idxes[2];
//					}
//					else {
//						update->tess_split_vertices[thread_idx][0] = verts_idxes[1];
//						update->tess_split_vertices[thread_idx][1] = verts_idxes[3];
//					}
//				}
//
//				increment();
//			}
//		};
//
//		switch (based_on){
//		case scme::TesselationModificationBasis::MODIFIED_POLYS: {
//
//			for (scme::ModifiedPoly& modified_poly : modified_polys) {
//
//				if (modified_poly.state == ModifiedPolyState::UPDATE &&
//					polys.isDeleted(modified_poly.idx) == false)
//				{
//					group_updates(modified_poly.idx);
//				}
//			}
//			break;
//		}
//
//		case scme::TesselationModificationBasis::MODIFIED_VERTICES: {
//
//			for (scme::ModifiedVertex& modified_v : modified_verts) {
//
//				if (modified_v.state == ModifiedVertexState::UPDATE &&
//					verts.isDeleted(modified_v.idx) == false)
//				{
//					Vertex& vertex = verts[modified_v.idx];
//
//					if (vertex.edge == 0xFFFF'FFFF) {
//						return;
//					}
//
//					// loop around the changed vertex to update all polygons connected to that vertex
//					uint32_t edge_idx = vertex.edge;
//					Edge* edge = &edges[edge_idx];
//
//					do {
//						group_updates(edge->p0);
//						group_updates(edge->p1);
//
//						// Iter
//						edge_idx = edge->nextEdgeOf(modified_v.idx);
//						edge = &edges[edge_idx];
//					} while (edge_idx != vertex.edge);
//				}
//			}
//			break;
//		}
//		}
//
//		uint32_t last_thread_idx = thread_idx;
//		
//		for (uint32_t i = last_thread_idx; i < 32; i++) {
//			update->tess_idxs[i][0] = 0xFFFFFFFF;
//		}
//
//		// Load
//		r.gpu_poly_normal_updates.upload(r.poly_normal_updates);
//
//		r.gpu_r_poly_normal_updates.resizeDiscard(r.poly_normal_updates.size());
//		r.poly_r_normal_updates.resize(r.poly_normal_updates.size());
//
//		// Command List
//		auto& ctx = r.im_ctx3;
//		ctx->ClearState();
//
//		// Shader Resources
//		{
//			std::array<ID3D11ShaderResourceView*, 2> srvs = {
//				r.gpu_poly_normal_updates.getSRV(),
//				gpu_verts.getSRV()
//			};
//			ctx->CSSetShaderResources(0, srvs.size(), srvs.data());
//		}
//
//		// UAVs
//		{
//			std::array<ID3D11UnorderedAccessView*, 2> uavs = {
//				gpu_triangles.getUAV(),
//				r.gpu_r_poly_normal_updates.getUAV()
//			};
//			ctx->CSSetUnorderedAccessViews(0, uavs.size(), uavs.data(), nullptr);
//		}
//
//		ctx->CSSetShader(r.update_tesselation_triangles.Get(), nullptr, 0);
//
//		ctx->Dispatch(r.poly_normal_updates.size(), 1, 1);
//
//		// Download Results
//		r.gpu_r_poly_normal_updates.download(r.poly_r_normal_updates, r.staging_buff);
//		
//		// Apply results to CPU
//		for (uint32_t group_idx = 0; group_idx < r.poly_normal_updates.size(); group_idx++) {
//
//			GPU_PolyNormalUpdateGroup& updates = r.poly_normal_updates[group_idx];
//			GPU_Result_PolyNormalUpdateGroup& results = r.poly_r_normal_updates[group_idx];
//
//			for (thread_idx = 0; thread_idx < 32; thread_idx++) {
//
//				if (group_idx == r.poly_normal_updates.size() - 1 &&
//					thread_idx == last_thread_idx)
//				{
//					break;
//				}
//
//				Poly& poly = polys[updates.tess_idxs[thread_idx][0] / 2];
//				poly.normal = results.poly_normal[thread_idx];
//				poly.tess_normals[0] = results.tess_normals[thread_idx][0];
//				poly.tess_normals[1] = results.tess_normals[thread_idx][1];
//			}
//		}
//	}
}

void SculptMesh::uploadInstances()
{
	const u32 thread_x_count = 12;

	if (gpu_instances.count() < instances.capacity()) {
		gpu_instances.resize(instances.capacity());
	}

	// Counting
	{
		uint32_t group_count = 0;
		uint32_t thread_count = 0;

		for (u32 i = 0; i < modified_instances.size(); i++) {

			thread_count++;

			if (thread_count == thread_x_count) {
				group_count++;
				thread_count = 0;
			}
		}

		if (group_count == 0) {
			group_count += 1;
		}

		gpu_instance_updates.resize(group_count);
		gpu_instance_updates.mapNoRead();
	}

	// Load Data
	{
		uint32_t group_idx = 0;
		uint32_t thread_idx = 0;

		GPU_InstanceUpdateGroup group = {};

		for (auto& modified_instance : modified_instances) {

			switch (modified_instance.state) {
			case scme::ModifiedInstanceType::UPDATE: {

				scme::Instance& instance = instances[modified_instance.idx];
				group.instance_idx[thread_idx] = modified_instance.idx + 1;

				auto& new_data = group.new_data[thread_idx];
				new_data.pos = dxConvert(instance.transform.pos);
				new_data.rot = dxConvert(instance.transform.rot);
				new_data.scale = dxConvert(instance.transform.scale);

				new_data.albedo_color = dxConvert(instance.material.albedo_color);
				new_data.roughness = instance.material.roughness;
				new_data.metallic = instance.material.metallic;
				new_data.specular = instance.material.specular;

				new_data.wireframe_front_color = dxConvert(instance.wireframe_colors.front_color);
				new_data.wireframe_back_color = dxConvert(instance.wireframe_colors.back_color);
				new_data.wireframe_tess_front_color = dxConvert(instance.wireframe_colors.tesselation_front_color);
				new_data.wireframe_tess_back_color = dxConvert(instance.wireframe_colors.tesselation_back_color);
				new_data.wireframe_tess_split_count = instance.wireframe_colors.tesselation_split_count;
				new_data.wireframe_tess_gap = instance.wireframe_colors.tesselation_gap;
				break;
			}

			case scme::ModifiedInstanceType::DELETED: {

				group.instance_idx[thread_idx] = modified_instance.idx + 1;
				group.new_data[thread_idx].scale = {};
				break;
			}
			}

			thread_idx++;

			if (thread_idx == thread_x_count) {

				gpu_instance_updates.update(group_idx, group);
				group = {};

				group_idx++;
				thread_idx = 0;
			}
		}

		if (group_idx == 0) {
			gpu_instance_updates.update(0, group);
		}

		gpu_instance_updates.unmap();
	}

	// Command List
	auto& r = renderer;
	auto& ctx = r.context;
	{
		ctx.beginCommandList();

		r.instance_update_call.CMD_bind();
		r.instance_update_call.CMD_setBufferParam(0, gpu_instance_updates);
		r.instance_update_call.CMD_setUnorderedAccessResourceParam(0, gpu_instances);
		r.instance_update_call.CMD_dispatch(
			(u32)gpu_instance_updates.count(), 1, 1);

		ctx.endAndWaitForCommandList();
	}
}
