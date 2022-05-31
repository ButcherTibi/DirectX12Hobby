#include "./App.hpp"


void App::_addTriangleMesh()
{
	auto& mesh = meshes.emplace_back();
	mesh.createAsTriangle(1);

	auto instance_idx = mesh.addInstance();
	auto& instance = mesh.instances[instance_idx];
	instance.transform = {};
	instance.material = {};
	instance.wireframe_colors = {};
}
