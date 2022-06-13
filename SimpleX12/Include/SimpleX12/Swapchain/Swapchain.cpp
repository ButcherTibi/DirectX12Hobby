#include "./Swapchain.hpp"


void Swapchain::create(Context* new_context, HWND hwnd)
{
	this->ctx = new_context;

	// Create Swapchain
	{
		DXGI_SWAP_CHAIN_DESC1 desc = {};
		desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		desc.Stereo = false;
		desc.SampleDesc.Count = 1;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.BufferCount = 2;
		desc.Scaling = DXGI_SCALING_NONE;
		desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		desc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
		desc.Flags = 0;

		checkDX12(ctx->factory->CreateSwapChainForHwnd(
			ctx->cmd_queue.Get(),
			hwnd,
			&desc,
			nullptr,
			nullptr,
			swapchain_1.GetAddressOf()
		));
	}

	// Backbuffers
	for (uint32_t idx = 0; idx < 2; idx++) {

		swapchain_1->GetBuffer(idx, IID_PPV_ARGS(buffs[idx].GetAddressOf()));
		backbuffers[idx].createSwapchainRenderTarget(ctx, buffs[idx].Get());
	}
}

void Swapchain::resize(uint32_t new_width, uint32_t new_height)
{
	swapchain_1->ResizeBuffers(2, new_width, new_height, DXGI_FORMAT_B8G8R8A8_UNORM, 0);

	for (uint32_t i = 0; i < 2; i++) {
		swapchain_1->GetBuffer(i, IID_PPV_ARGS(buffs[i].GetAddressOf()));
		backbuffers[i].createSwapchainRenderTarget(ctx, buffs[i].Get());
	}
}

uint32_t Swapchain::getBackbufferIndex()
{
	return index;
}

void Swapchain::transitionToRenderTarget()
{
	backbuffers[index].transitionTo(D3D12_RESOURCE_STATE_RENDER_TARGET);
}

void Swapchain::transitionToPresentation()
{
	backbuffers[index].transitionTo(D3D12_RESOURCE_STATE_PRESENT);
}

void Swapchain::present()
{
	swapchain_1->Present(0, 0);
	index = (index + 1) % 2;
}
