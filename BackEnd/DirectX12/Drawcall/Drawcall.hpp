#pragma once

#include <DirectX12/Context/Context.hpp>
#include <DirectX12/Resource/Resource.hpp>
#include <DirectX12/Shader/Shader.hpp>
#include <DirectX12/Descriptors/Descriptors.hpp>


class CallBase {
protected:
	std::vector<D3D12_ROOT_PARAMETER> params;
	ComPtr<ID3D12RootSignature> root_signature;

protected:
	std::string buildRootSiganture();

public:
	void setShaderResourceViewParam(uint32_t shader_register, D3D12_SHADER_VISIBILITY shader_visibility = D3D12_SHADER_VISIBILITY_ALL);
	void setUnorderedAccessViewParam(uint32_t shader_register, D3D12_SHADER_VISIBILITY shader_visibility = D3D12_SHADER_VISIBILITY_ALL);
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
	void init();

	void setVertexShader(VertexShader* vertex_shader);

	void setPixelShader(PixelShader* pixel_shader);

	void build();


	/* Commands ******************************************************************************/

	void CMD_bind();

	void CMD_setShaderResourceView(uint32_t shader_register, Resource* resource);

	void CMD_setIndexBuffer(IndexBuffer& index_buffer);

	void CMD_setViewportSize(float width, float height);
	void CMD_setViewportSize(uint32_t width, uint32_t height);

	void CMD_setRenderTargets(std::vector<DescriptorHandle> render_targets);

	void CMD_clearRenderTarget(DescriptorHandle render_target, float red = 0, float green = 0, float blue = 0, float alpha = 0);

	void CMD_draw(uint32_t vertex_count, uint32_t instance_count = 1);
	void CMD_drawIndexed(uint32_t instance_count = 1);
};


class DispatchCall : public CallBase {
	D3D12_COMPUTE_PIPELINE_STATE_DESC pipe_desc;
	ComputeShader* shader;
	ComPtr<ID3D12PipelineState> pipeline;

public:
	void init();

	void setComputeShader(ComputeShader* compute_shader);

	void build();


	/* Commands ******************************************************************************/

	void CMD_bind();

	void CMD_setShaderResourceView(uint32_t shader_register, Resource* resource);

	void CMD_setUnorderedAccessView(uint32_t shader_register, Resource* resource);

	void CMD_dispatch(uint32_t thread_group_count_x = 1, uint32_t thread_group_count_y = 1, uint32_t thread_group_count_z = 1);
};
