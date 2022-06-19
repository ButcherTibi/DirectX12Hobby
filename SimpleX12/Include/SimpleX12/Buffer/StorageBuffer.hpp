#pragma once
#include "./Buffer.hpp"


template<typename GPU_T = float>
class StorageBuffer : public Buffer {
public:
	void resize(uint32_t new_count)
	{
		Resource::resize(new_count * sizeof(GPU_T));
	}

	void update(uint32_t index, GPU_T& element)
	{
		Resource::update(index, &element, sizeof(GPU_T));
	}

	uint32_t count()
	{
		return (uint32_t)(desc.Width / sizeof(GPU_T));
	}
};