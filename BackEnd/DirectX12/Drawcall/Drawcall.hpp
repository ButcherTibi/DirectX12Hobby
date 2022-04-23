#pragma once

#include <variant>

#include <DirectX12/Context/Context.hpp>
#include <DirectX12/Resource/Resource.hpp>
#include <DirectX12/Shader/Shader.hpp>
#include <DirectX12/Descriptors/Descriptors.hpp>


struct RootSignatureParam {
	D3D12_ROOT_PARAMETER_TYPE type;
	uint32_t shader_register;
};

class Drawcall {
	std::vector<D3D12_ROOT_PARAMETER> params;
	ComPtr<ID3D12RootSignature> root_signature;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipe_desc;
	std::string hlsl_root_signature;
	VertexShader* vertex_shader = nullptr;
	PixelShader* pixel_shader = nullptr;
	ComPtr<ID3D12PipelineState> pipeline;

	// Commands
	ID3D12GraphicsCommandList* cmd_list;

	D3D12_VIEWPORT viewport;
	D3D12_RECT scissor;

	bool is_indexed = false;
	uint32_t vertex_count = 0xFFFF'FFFF;
	uint32_t instance_count;

	// State

private:
	void build();

public:
	void init();

	void setShaderResourceViewParam(uint32_t shader_register, D3D12_SHADER_VISIBILITY shader_visibility = D3D12_SHADER_VISIBILITY_ALL);

	void setVertexShader(VertexShader* vertex_shader);

	void setPixelShader(PixelShader* pixel_shader);

	void draw(uint32_t new_vertex_count, uint32_t new_instance_count = 1);


	/* Commands ******************************************************************************/

	void beginCmd(ID3D12GraphicsCommandList* cmd_list);

	void setShaderResourceViewCmd(uint32_t shader_register, Resource* resource);

	void setViewportSizeCmd(float width, float height);

	void setRenderTargets(std::vector<DescriptorHandle>& render_targets);

	void clearRenderTarget(DescriptorHandle render_target, float red, float green, float blue, float alpha);

	void endCmd();
};
