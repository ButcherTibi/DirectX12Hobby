// Header
#include "DreamGraphicsAPI.hpp"

using namespace dga;


uint32_t findMemoryTypeIndex(Device* device, uint32_t mem_type_bits, VkMemoryPropertyFlags properties)
{
	for (uint32_t i = 0; i < device->mem_props.memoryTypeCount; i++) {

		VkMemoryType mem_type = device->mem_props.memoryTypes[i];

		bool is_memory_type = mem_type_bits & (1 << i);
		bool is_memory_prop = (mem_type.propertyFlags & properties) == properties;

		if (is_memory_type && is_memory_prop) {
			return i;
		}
	}

	throw;
}

void Device::createTexture(uint32_t width, uint32_t height, VkFormat format, Texture* texture)
{
	texture->device = this;
	texture->width = width;
	texture->height = height;
	texture->format = format;

	// Image
	{
		VkImageCreateInfo info{};
		info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		info.imageType = VK_IMAGE_TYPE_2D;
		info.format = format;
		info.extent.width = width;
		info.extent.height = height;
		info.extent.depth = 1;
		info.mipLevels = 1;
		info.arrayLayers = 1;
		info.samples = VK_SAMPLE_COUNT_1_BIT;
		info.tiling = VK_IMAGE_TILING_OPTIMAL;
		info.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			// VK_IMAGE_USAGE_TRANSFER_DST_BIT |
			// VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT |
			// VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		info.queueFamilyIndexCount = 0;
		info.pQueueFamilyIndices = nullptr;
		info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

		texture->usage = info.usage;
		texture->layout = info.initialLayout;

		if (vkCreateImage(device, &info, nullptr, &texture->image) != VK_SUCCESS) {
			throw;
		}
	}
	
	// Memory
	{
		VkMemoryRequirements mem_req;
		vkGetImageMemoryRequirements(device, texture->image, &mem_req);

		VkMemoryAllocateInfo info{};
		info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		info.allocationSize = mem_req.size;
		info.memoryTypeIndex = findMemoryTypeIndex(this, mem_req.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		if (vkAllocateMemory(device, &info, nullptr, &texture->memory) != VK_SUCCESS) {
			throw;
		}

		vkBindImageMemory(device, texture->image, texture->memory, 0);
	}
}

void Texture::createTextureView(TextureView* r_texture_view)
{
	r_texture_view->texture = this;
	r_texture_view->format = format;
	r_texture_view->view = nullptr;

	VkImageViewCreateInfo info{};
	info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	info.image = image;
	info.viewType = VK_IMAGE_VIEW_TYPE_2D;
	info.format = format;
	info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	info.subresourceRange.baseMipLevel = 0;
	info.subresourceRange.levelCount = 1;
	info.subresourceRange.baseArrayLayer = 0;
	info.subresourceRange.layerCount = 1;

	if (vkCreateImageView(device->device, &info, nullptr, &r_texture_view->view) != VK_SUCCESS) {
		throw;
	}
}

Texture::~Texture()
{
	if (device != nullptr) {
		vkDestroyImage(device->device, image, nullptr);
		vkFreeMemory(device->device, memory, nullptr);
	}
}

TextureView::~TextureView()
{
	if (texture != nullptr) {
		vkDestroyImageView(texture->device->device, view, nullptr);
	}
}
