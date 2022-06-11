#pragma once

#include "../Descriptors/Descriptors.hpp"
#include "../Shader/Shader.hpp"
#include "../Buffer/IndexBuffer.hpp"
#include "../Buffer/ConstantBuffer.hpp"


enum class ShaderInputType {
	ConstantBuffer,
	Buffer,
	Texture,
	UnorderedAccessResource
};

struct ShaderParameter {
	ShaderInputType type;
	uint32_t shader_register;
	D3D12_SHADER_VISIBILITY shader_visibility;
};

class CallBase {
protected:
	Context* context = nullptr;
	std::vector<ShaderParameter> params;

	ComPtr<ID3D12RootSignature> root_signature;
	std::string hlsl_root_signature;

	// Command State
	std::vector<DescriptorHeap*> used_heaps;

protected:
	std::string buildRootSiganture();

public:
	void setConstantBufferParam(uint32_t b_register, D3D12_SHADER_VISIBILITY shader_visibility = D3D12_SHADER_VISIBILITY_ALL);
	void setBufferParam(uint32_t t_register, D3D12_SHADER_VISIBILITY shader_visibility = D3D12_SHADER_VISIBILITY_ALL);
	void setTextureParam(uint32_t t_register, D3D12_SHADER_VISIBILITY = D3D12_SHADER_VISIBILITY_ALL);
	void setUnorderedAccessResourceParam(uint32_t u_register, D3D12_SHADER_VISIBILITY shader_visibility = D3D12_SHADER_VISIBILITY_ALL);
};


class Drawcall : public CallBase {
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipe_desc;
	VertexShader* vertex_shader = nullptr;
	PixelShader* pixel_shader = nullptr;
	ComPtr<ID3D12PipelineState> pipeline;

	// Commands
	D3D12_VIEWPORT viewport;
	D3D12_RECT scissor;
	uint32_t index_buff_count = 0xFFFF'FFFF;

	// State

public:
	void create(Context* context);

	void setVertexShader(VertexShader* vertex_shader);

	void setPixelShader(PixelShader* pixel_shader);

	void setRenderTargetFormats(DXGI_FORMAT rtv_format_0);

	void rebuild();


	/* Commands ******************************************************************************/

	void CMD_bind();

	void CMD_setIndexBuffer(IndexBuffer& index_buffer);
	
	void CMD_setConstantBufferParam(uint32_t b_register, ConstantBuffer& const_buffer);
	void CMD_setBufferParam(uint32_t t_register, Buffer& buffer);
	void CMD_setTextureParam(uint32_t t_register, SRV_DescriptorHandle& texture_handle);

	void CMD_setViewportSize(float width, float height);
	void CMD_setViewportSize(uint32_t width, uint32_t height);

	void CMD_setRenderTargets(std::vector<RTV_DescriptorHandle> render_targets);

	void CMD_clearRenderTarget(RTV_DescriptorHandle render_target, float red = 0, float green = 0, float blue = 0, float alpha = 0);

	void CMD_draw(uint32_t vertex_count = 3, uint32_t instance_count = 1);
	void CMD_drawIndexed(uint32_t instance_count = 1);
};


class DispatchCall : public CallBase {
	D3D12_COMPUTE_PIPELINE_STATE_DESC pipe_desc;
	ComputeShader* shader;
	ComPtr<ID3D12PipelineState> pipeline;

public:
	void create(Context* context);

	void setComputeShader(ComputeShader* compute_shader);

	void rebuild();


	/* Commands ******************************************************************************/

	void CMD_bind();

	void CMD_setBufferParam(uint32_t t_register, Buffer& resource);
	void CMD_setUnorderedAccessResourceParam(uint32_t u_register, Resource& resource);

	void CMD_dispatch(uint32_t thread_group_count_x = 1, uint32_t thread_group_count_y = 1, uint32_t thread_group_count_z = 1);
};
