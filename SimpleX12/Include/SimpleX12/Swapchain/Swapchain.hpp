#pragma once

#include "../Texture/Texture.hpp"

#include <array>


class Swapchain {
	Context* ctx = nullptr;

	ComPtr<IDXGISwapChain1> swapchain_1;
	std::array<ComPtr<ID3D12Resource>, 2> buffs;
	uint32_t index = 0;

public:
	std::array<Texture, 2> backbuffers;

public:
	void create(Context* context, HWND hwnd);

	void resize(uint32_t new_width, uint32_t new_height);

	uint32_t getBackbufferIndex();

	void transitionToRenderTarget();
	void transitionToPresentation();

	void present();
};
