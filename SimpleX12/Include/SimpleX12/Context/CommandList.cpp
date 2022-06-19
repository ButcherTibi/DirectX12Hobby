#include "./Context.hpp"

#include <array>

#include "../Descriptors/Descriptors.hpp"


void CommandList::setDescriptorHeaps(DescriptorHeap& heap_0)
{
	std::array<ID3D12DescriptorHeap*, 1> heaps = {
		heap_0.get()
	};
	context->cmd_list->SetDescriptorHeaps((uint32_t)heaps.size(), heaps.data());
}

void CommandList::clearRenderTarget(RTV_DescriptorHandle render_target, float red, float green, float blue, float alpha)
{
	const float clearColor[] = { red, green, blue, alpha };
	context->cmd_list->ClearRenderTargetView(render_target.cpu_handle, clearColor, 0, nullptr);
}

RecordCommandList::RecordCommandList(Context* new_context)
{
	context = new_context;

	if (context != nullptr) {
		context->beginCommandList();
	}
}

RecordCommandList::~RecordCommandList()
{
	if (context != nullptr) {
		context->endAndWaitForCommandList();
	}
}

RecordAndWaitCommandList::RecordAndWaitCommandList(Context* new_context)
{
	context = new_context;

	if (context != nullptr) {
		context->beginCommandList();
	}
}

RecordAndWaitCommandList::~RecordAndWaitCommandList()
{
	if (context != nullptr) {
		context->endAndWaitForCommandList();
	}
}
