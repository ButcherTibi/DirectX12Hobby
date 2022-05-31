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


struct State {
	u32 render_width = 250;
	u32 render_height = 250;

	bool capture_frame = false;
};

struct Request {

};

struct AddTriangleMesh : Request {

};

typedef std::variant<
	AddTriangleMesh
> AsyncRequest;


/* which subprimitive holds the surface data to respond to the light */
enum class GPU_ShadingNormal : uint32_t {
	VERTEX,
	POLY,
	TESSELATION  // quads are split into 2 triangles and each has a normal
};


class App {
public:
	std::mutex state_swap_lock;
	State prev_state;
	State state;
	State next_state;

	std::mutex requests_mutex;
	std::list<AsyncRequest> requests;

	std::list<scme::SculptMesh> meshes;

	// Shading
	GPU_ShadingNormal shading_normal;  // what normal to use when shading the mesh in the pixel shader

private:
	//void tick();
	//void loop();

	void _addTriangleMesh();

public:
	void init();


	/* Aplication loop is made from these methods which have a timing constraint */

	void phase_1_runCPU();
	void phase_2_waitForRendering();
	bool phase_2X_tryDownloadRender(u32 width, u32 height, byte* r_pixels);
	void phase_3_render();


	/* These methods do not require timing */

	void captureFrame();

	void addTriangleMesh();
};
extern App app;
