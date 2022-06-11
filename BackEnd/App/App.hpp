#pragma once

// Standard
#include <chrono>
namespace chrono = std::chrono;
#include <mutex>
#include <array>
#include <list>
#include <variant>

#include <CommonTypes.hpp>
#include <Sculpt Mesh/SculptMesh.hpp>
#include <Renderer/Renderer.hpp>


/* which subprimitive holds the surface data to respond to the light */
enum class GPU_ShadingNormal : u32 {
	VERTEX,
	POLY,
	TESSELATION  // quads are split into 2 triangles and each has a normal
};

// Light don't have a position and rotate with the camera
struct Light {
	glm::vec3 normal;
	glm::vec3 color;
	float intensity;
};

struct Lighting {
	// what normal to use when shading the mesh in the pixel shader
	GPU_ShadingNormal shading_normal;
	std::array<Light, 8> lights;
	float ambient_intensity;
};

struct Camera {
	// 3D position in space where the camera is starring at,
	// used to tweak camera sensitivity to make movement consistent across al mesh sizes
	glm::vec3 focal_point;

	// horizontal field of view used in the perspective matrix
	float field_of_view;

	// defines the clipping planes used in rendering (does not affect depth)
	float z_near;
	float z_far;

	// camera position in global space
	glm::vec3 pos;

	// reverse camera rotation applied in vertex shader
	glm::quat quat_inv;

	// camera's direction of pointing
	glm::vec3 forward;

	// camera movement sensitivity
	float orbit_sensitivity;
	float pan_sensitivity;
	float dolly_sensitivity;
};

struct Viewport {
	u32 width;
	u32 height;

	Camera camera;
	Lighting lighting;
};

struct DebugStuff {
	bool capture_frame;
};

class App {
	friend class Renderer;

	std::mutex state_update_lock;

	std::list<scme::SculptMesh> meshes;

	Viewport viewport;
	DebugStuff debug;

private:


public:
	void init();


	/* Aplication loop is made from these methods which have a timing constraint */

	void phase_1_runCPU();
	void phase_2_waitForRendering();
	bool phase_2X_tryDownloadRender(u32 width, u32 height, byte* r_pixels);
	void phase_3_render();


	/* Mesh **************************************************/

	//


	/* Instance **********************************************/

	void createTriangleInstance();


	/* Camera ************************************************/

	// sets the camera point of focus to adjust sensitivity
	void setCameraFocalPoint(glm::vec3& new_focus);

	// orbit camera around focus
	void arcballOrbitCamera(float deg_x, float deg_y);

	// moves the camera along the camera's up and right axes
	void panCamera(float amount_x, float amount_y);

	// moves the camera along the camera's forward axis
	void dollyCamera(float amount);

	// sets the camera's global position
	void setCameraPosition(float x, float y, float z);

	// sets the camera's rotation
	// arguments are Euler and get converted to quartenion
	void setCameraRotation(float x, float y, float z);


	/* Debug ************************************************/

	void captureFrame();
};
extern App app;
