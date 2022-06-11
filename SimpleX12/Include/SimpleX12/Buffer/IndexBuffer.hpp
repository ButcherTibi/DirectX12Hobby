#pragma once

#include "../Resource/Resource.hpp"


class IndexBuffer : public Buffer {
public:
	void create(Context* new_context, D3D12_HEAP_TYPE heap_type = D3D12_HEAP_TYPE_DEFAULT);

	void resize(uint32_t new_count);

	void update(uint32_t index, uint32_t element);

	uint32_t count();
};
