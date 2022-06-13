#pragma once

// Standard
#include <chrono>
namespace chrono = std::chrono;
#include <mutex>
#include <array>
#include <list>
#include <variant>

// Sub Components
#include "Window/Window.hpp"

#include <CommonTypes.hpp>
#include <Sculpt Mesh/SculptMesh.hpp>
#include <Renderer/Renderer.hpp>
#include "Input.hpp"
#include "Camera/Camera.hpp"


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

struct Viewport {
	u32 width;
	u32 height;

	bool pan_started = false;
};

struct DebugStuff {
	bool capture_frame;
};

class App {
public:
	std::mutex state_update_lock;

	std::thread thread;  // this is the thread that runs the backend

	Window window;
	Input input;
	
	// Timing
	SteadyTime frame_start_time;
	float delta_time;  // the total duration of the last frame
	u32 min_frame_duration_ms;  // the minimum amount of time a frame must last (60 fps limit)

	std::list<scme::SculptMesh> meshes;

	Viewport viewport;
	Camera camera;
	Lighting lighting;

	DebugStuff debug;

private:
	void main(bool enable_pix_debugger);
	void CPU_update();

public:
	void init(bool enable_pix_debugger);


	/* Mesh **************************************************/

	//


	/* Instance **********************************************/

	void createTriangleInstance();


	/* Debug ************************************************/

	void captureFrame();
};
extern App app;
