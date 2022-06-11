#include "./App.hpp"

// #include <glm/common.hpp>


void App::setCameraFocalPoint(glm::vec3& focal_point)
{
	std::lock_guard lock{ state_update_lock };
	viewport.camera.focal_point = focal_point;
}

// TODO: ask stack exchange about below

/* Attempt 1: */
/*glm::quat rotation_x = glm::rotate(glm::quat(1, 0, 0, 0), toRad(deg_x), { 1, 0, 0 });
glm::quat rotation_y = glm::rotate(glm::quat(1, 0, 0, 0), toRad(deg_y), { 0, 1, 0 });
mesh.rot = glm::normalize(mesh.rot * rotation_x * rotation_y);*/

/* Attempt 2: */
/*mesh.rot = glm::rotate(mesh.rot, toRad(deg_x), { 1, 0, 0 });
mesh.rot = glm::rotate(mesh.rot, toRad(deg_y), { 0, 1, 0 });
mesh.rot = glm::normalize(mesh.rot);*/

/* Attempt 3: Rotation Around X Local, Rotation Around Y Screen */
/*mesh.rot = glm::rotate(mesh.rot, toRad(deg_x), { 1, 0, 0 });

glm::vec3 y_axis = { 0, 1, 0 };
y_axis = y_axis * mesh.rot;
mesh.rot = glm::rotate(mesh.rot, toRad(deg_y), y_axis);

mesh.rot = glm::normalize(mesh.rot);*/

/* Attempt 4: Rotation Around X Screen, Rotation Around Y Local */
/*glm::vec3 x_axis = { 1, 0, 0 };
x_axis = x_axis * mesh.rot;
mesh.rot = glm::rotate(mesh.rot, toRad(deg_x), x_axis);
mesh.rot = glm::rotate(mesh.rot, toRad(deg_y), { 0, 1, 0 });

mesh.rot = glm::normalize(mesh.rot);*/

/* Attempt 5: Rotation Around X Screen, Rotation Around Y Screen */
/*glm::vec3 x_axis = { 1, 0, 0 };
x_axis = x_axis * mesh.rot;
mesh.rot = glm::rotate(mesh.rot, toRad(deg_x), x_axis);

glm::vec3 y_axis = { 0, 1, 0 };
y_axis = y_axis * mesh.rot;
mesh.rot = glm::rotate(mesh.rot, toRad(deg_y), y_axis);

mesh.rot = glm::normalize(mesh.rot);*/

// if I rotate a quaternion by position then clusterfuck

void App::arcballOrbitCamera(float deg_x, float deg_y)
{
	std::lock_guard lock{ state_update_lock };

	auto& camera_quat_inv = viewport.camera.quat_inv;
	auto& camera_focus = viewport.camera.focal_point;
	auto& camera_pos = viewport.camera.pos;
	auto& camera_forward = viewport.camera.forward;

	glm::vec3 camera_right = glm::normalize(glm::vec3{ 1, 0, 0 } * camera_quat_inv);
	glm::vec3 camera_up = glm::normalize(glm::vec3{ 0, 1, 0 } * camera_quat_inv);

	glm::quat delta_rot = { 1, 0, 0, 0 };
	delta_rot = glm::rotate(delta_rot, toRad(deg_y), camera_right);
	delta_rot = glm::rotate(delta_rot, toRad(deg_x), camera_up);
	delta_rot = glm::normalize(delta_rot);

	camera_pos = (camera_pos - camera_focus) * delta_rot;
	camera_pos += camera_focus;

	glm::quat reverse_rot = camera_quat_inv;
	reverse_rot = glm::rotate(reverse_rot, toRad(deg_y), camera_right);
	reverse_rot = glm::rotate(reverse_rot, toRad(deg_x), camera_up);

	camera_quat_inv = reverse_rot;

	camera_forward = glm::normalize(glm::vec3{ 0, 0, -1 } * camera_quat_inv);
}

void App::panCamera(float amount_x, float amount_y)
{
	std::lock_guard lock{ state_update_lock };

	auto& camera_quat_inv = viewport.camera.quat_inv;
	auto& camera_focus = viewport.camera.focal_point;
	auto& camera_pos = viewport.camera.pos;

	glm::vec3 camera_right = glm::normalize(glm::vec3{ 1, 0, 0 } * camera_quat_inv);
	glm::vec3 camera_up = glm::normalize(glm::vec3{ 0, 1, 0 } * camera_quat_inv);

	float dist = glm::distance(camera_focus, camera_pos);
	if (!dist) {
		dist = 1;
	}

	camera_pos += amount_x * dist * camera_right + (-amount_y) * dist * camera_up;
}

void App::dollyCamera(float amount)
{
	std::lock_guard lock{ state_update_lock };

	auto& camera_focus = viewport.camera.focal_point;
	auto& camera_pos = viewport.camera.pos;
	auto& camera_forward = viewport.camera.forward;

	// glm::vec3 camera_forward_axis = glm::normalize(glm::vec3{ 0, 0, -1 } * camera_quat_inv);

	float dist = glm::distance(camera_focus, camera_pos);
	if (!dist) {
		dist = 1;
	}

	camera_pos += amount * dist * camera_forward;
}

void App::setCameraPosition(float x, float y, float z)
{
	std::lock_guard lock{ state_update_lock };
	viewport.camera.pos = { x, y, z };
}

void App::setCameraRotation(float x, float y, float z)
{
	std::lock_guard lock{ state_update_lock };

	auto& camera_quat_inv = viewport.camera.quat_inv;
	auto& camera_forward = viewport.camera.forward;

	glm::quat x_rot = glm::rotate(glm::quat{ 1, 0, 0, 0 }, x, { 1, 0, 0 });
	glm::quat y_rot = glm::rotate(glm::quat{ 1, 0, 0, 0 }, y, { 0, 1, 0 });
	glm::quat z_rot = glm::rotate(glm::quat{ 1, 0, 0, 0 }, z, { 0, 0, 1 });

	camera_quat_inv = glm::normalize(x_rot * y_rot * z_rot);

	camera_forward = glm::normalize(glm::vec3{ 0, 0, -1 } *camera_quat_inv);
}
