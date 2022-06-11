#include "App.hpp"


void App::createTriangleInstance()
{
	std::lock_guard lock{ state_update_lock };

	auto& mesh = meshes.emplace_back();
	mesh.createAsTriangle(1);

	auto insta_idx = mesh.addInstance();
	mesh.instances[insta_idx].transform = {};
	mesh.instances[insta_idx].material = {};
	mesh.instances[insta_idx].wireframe_colors = {};
}