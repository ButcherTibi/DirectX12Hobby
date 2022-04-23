#pragma once

// Standard
#include <array>
#include <vector>

// Vulkan
#include "vulkan/vulkan.h"


namespace dga {

	class Device;
	class Texture;
	class CommandBuffer;

	/* Common Base Objects */

	class NotCopyable {
	public:
		NotCopyable() = default;
		NotCopyable(NotCopyable const&) = delete;
		NotCopyable& operator=(NotCopyable const&) = delete;
	};

	class DeviceObject {
	public:
		Device* device = nullptr;
	};


	/* Resources */

	class Buffer {

	};


	class TextureView {
	public:
		Texture* texture;

		VkFormat format;

		VkImageView view;

		~TextureView();
	};

	class Texture : public DeviceObject, NotCopyable {
	public:
		uint32_t width;
		uint32_t height;
		VkFormat format;
		VkImageUsageFlags usage;
		VkImageLayout layout;

		VkImage image;
		VkDeviceMemory memory;

	public:
		// void resize(uint32_t width, uint32_t height);

		void createTextureView(TextureView* r_texture_view);

		~Texture();
	};


	class Shader : public DeviceObject , NotCopyable {
	public:
		VkShaderModule shader;

	public:
		~Shader();
	};

	class VertexShader : public Shader { };
	class PixelShader : public Shader { };


	/* Management */

	struct Attachment {
		TextureView* tex_view;

		uint32_t vertex_shader_input_index;
		uint32_t pixel_shader_input_index;
		uint32_t render_target_index;
	};

	void setToDefault(VkPipelineColorBlendAttachmentState& blend_state);
	void setToAlphaBlending(VkPipelineColorBlendAttachmentState& blend_state);

	class RasterizerPipeline : public DeviceObject, NotCopyable {
	public:
		// Input Assembly
		VkPipelineVertexInputStateCreateInfo vertex_input_info;
		VkPipelineInputAssemblyStateCreateInfo input_assembly_info;

		// Vertex Shader
		VkPipelineShaderStageCreateInfo vertex_stage_info;

		// Rasterizer
		VkViewport viewport_info;
		VkRect2D scissor_info;
		VkPipelineViewportStateCreateInfo viewport_state_info;
		VkPipelineRasterizationStateCreateInfo rasterizer_info;
		VkPipelineMultisampleStateCreateInfo multisample_info;

		// Pixel Shader
		VkPipelineShaderStageCreateInfo pixel_stage_info;

		// Blend State
		std::vector<VkPipelineColorBlendAttachmentState> blend_attachments_infos;
		VkPipelineColorBlendStateCreateInfo blend_state_info;

		std::vector<VkPipelineShaderStageCreateInfo> stage_infos;

		// Other
		VkPipelineLayoutCreateInfo pipe_layout_info;
		VkGraphicsPipelineCreateInfo pipeline_info;

		std::vector<Attachment> atachments;
		uint32_t vertex_shader_input_count;
		uint32_t pixel_shader_input_count;
		uint32_t render_target_index_count;

		uint32_t default_width;
		uint32_t default_height;

		// RenderPass
		std::vector<VkAttachmentDescription> attach_descps;
		std::vector<VkAttachmentReference> input_attach_refs;
		std::vector<VkAttachmentReference> color_attach_refs;
		std::vector<VkAttachmentReference> resolve_attach_refs;
		VkSubpassDescription subpass_desc;
		VkRenderPass renderpass;

		// Framebuffer
		std::vector<VkImageView> views;
		VkFramebuffer framebuffer;

		VkPipelineLayout pipe_layout;
		VkPipeline pipeline;

	public:
		void reset();

		// Input Assembly
		void setPrimitiveTopology(VkPrimitiveTopology topology);

		// Vertex Shader
		//void setVertexShaderTextureInput(uint32_t input_index, TextureView* texture_view);
		void setVertexShader(VertexShader* vertex_shader);
		

		/* Rasterizer */
		
		void setViewportPosition(float x, float y);
		void setViewportSize(float width, float height);
		void setViewportDepth(float min_depth, float max_depth);
		void setScissorOffset(int32_t x, int32_t y);
		void setScissorExtend(uint32_t width, uint32_t height);

		void setPolygonMode(VkPolygonMode mode);
		void setCullMode(VkCullModeFlags flags);
		void setFrontFace(VkFrontFace front_face);

		void setMultisampleCount(VkSampleCountFlagBits flags);

		// @TODO: Depth test

		// Pixel Shader
		//void setPixelShaderTextureInput(uint32_t input_index, TextureView* texture_view);
		void setPixelShader(PixelShader* pixel_shader);


		/* Blend State */

		void addRenderTarget(TextureView* texture_view);
		void addRenderTarget(TextureView* texture_view, const VkPipelineColorBlendAttachmentState& blend_state);


		/* Commands */

		// void draw();


		void _createVkObjects();
		void _recordCommands(CommandBuffer* command_buffer);

		~RasterizerPipeline();
	};



	class CommandBuffer : public DeviceObject, NotCopyable {
	public:
		RasterizerPipeline* rasterizer_pipeline;

		VkCommandBuffer cmd_buff;

	public:
		void bindRasterizerPipeline(RasterizerPipeline* rasterizer_pipeline);

		void submit();

		~CommandBuffer();
	};


	class Device : NotCopyable {
	public:
		VkDevice device = nullptr;
		VkQueue graphics_queue = nullptr;
		VkCommandPool cmd_pool = nullptr;

		VkPhysicalDeviceMemoryProperties mem_props;

		void _createShader(std::vector<char8_t>& shader_spirv, Shader& r_shader);

	public:
		void createVertexShader(std::vector<char8_t>& shader_spirv, VertexShader& r_vertex_shader);
		void createPixelShader(std::vector<char8_t>& shader_spirv, PixelShader& r_pixel_shader);

		void createTexture(uint32_t width, uint32_t height, VkFormat format, Texture* texture);

		void createCommandBuffer(CommandBuffer& r_command_buffer);
		void createRasterizerPipeline(RasterizerPipeline& r_pipeline);

		~Device();
	};


	class Instance : NotCopyable {
	public:
		VkInstance instance = nullptr;

		std::array<const char*, 1> layers = {
			"VK_LAYER_KHRONOS_validation"
		};

		std::array<const char*, 1> extensions = {
			VK_EXT_DEBUG_UTILS_EXTENSION_NAME
		};

		VkDebugUtilsMessengerEXT callback = VK_NULL_HANDLE;
		PFN_vkSetDebugUtilsObjectNameEXT setDebugUtilsObjectName;

	public:
		void create();

		void getBestDevice(Device& r_device);

		~Instance();
	};
}
