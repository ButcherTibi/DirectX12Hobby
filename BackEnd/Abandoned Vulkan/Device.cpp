// Header
#include "DreamGraphicsAPI.hpp"

using namespace dga;


void Device::_createShader(std::vector<char8_t>& code, Shader& r_shader)
{
	r_shader.device = this;

	VkShaderModuleCreateInfo info{};
	info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	info.codeSize = code.size();
	info.pCode = reinterpret_cast<const uint32_t*>(code.data());

	if (vkCreateShaderModule(device, &info, nullptr, &r_shader.shader) != VK_SUCCESS) {
		throw;
	}
}

void Device::createVertexShader(std::vector<char8_t>& compiled_shader_code, VertexShader& r_vertex_shader)
{
	_createShader(compiled_shader_code, r_vertex_shader);
}

void Device::createPixelShader(std::vector<char8_t>& compiled_shader_code, PixelShader& r_pixel_shader)
{
	_createShader(compiled_shader_code, r_pixel_shader);
}

void Device::createCommandBuffer(CommandBuffer& r_command_buffer)
{
	r_command_buffer.device = this;
	r_command_buffer.cmd_buff = nullptr;

	// Command Buffer
	{
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = cmd_pool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = 1;

		if (vkAllocateCommandBuffers(device, &allocInfo, &r_command_buffer.cmd_buff) != VK_SUCCESS) {
			throw;
		}
	}
}

void Device::createRasterizerPipeline(RasterizerPipeline& r_pipeline)
{
	r_pipeline.device = this;
	r_pipeline.reset();
}

Device::~Device()
{
	if (device != nullptr) {
		vkDestroyCommandPool(device, cmd_pool, nullptr);
		vkDestroyDevice(device, nullptr);
	}
}
