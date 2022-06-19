#pragma once

#include "../Resource/Resource.hpp"


class Buffer : public Resource {
public:
	void create(Context* context,
		D3D12_HEAP_TYPE heap_type = D3D12_HEAP_TYPE_DEFAULT,
		D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE
	);

	// @HERE: read the world pos pixel to se camera focal point for nice zoom
	/// <summary>
	/// 
	/// </summary>
	// void readPixel(uint32_t index, std::array<uint32_t, 4>& mem);
};
