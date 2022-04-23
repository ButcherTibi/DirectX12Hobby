// Header
#include "DreamGraphicsAPI.hpp"

using namespace dga;

void CommandBuffer::bindRasterizerPipeline(RasterizerPipeline* new_rasterizer_pipeline)
{
	rasterizer_pipeline = new_rasterizer_pipeline;

	rasterizer_pipeline->_createVkObjects();
}

void CommandBuffer::submit()
{
	VkCommandBufferBeginInfo begin_info{};
	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	begin_info.pInheritanceInfo = nullptr; // Optional

	if (vkBeginCommandBuffer(cmd_buff, &begin_info) != VK_SUCCESS) {
		throw;
	}

	rasterizer_pipeline->_recordCommands(this);

	if (vkEndCommandBuffer(cmd_buff) != VK_SUCCESS) {
		throw;
	}

	{
		VkSubmitInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		info.waitSemaphoreCount = 0;
		info.pWaitSemaphores = nullptr;
		info.pWaitDstStageMask = nullptr;
		info.commandBufferCount = 1;
		info.pCommandBuffers = &cmd_buff;
		info.signalSemaphoreCount = 0;
		info.pSignalSemaphores = nullptr;

		if (vkQueueSubmit(device->graphics_queue, 1, &info, nullptr) != VK_SUCCESS) {
			throw;
		}
	}
}

CommandBuffer::~CommandBuffer()
{
	
}
