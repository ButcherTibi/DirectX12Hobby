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
	ID3D12GraphicsCommandList* cmd_list = nullptr;

	D3D12_VIEWPORT viewport;
	D3D12_RECT scissor;

	// State

private:
	void assertCmdListUnset();
	void assertCmdListSet();

public:
	void init();

	void setShaderResourceViewParam(uint32_t shader_register, D3D12_SHADER_VISIBILITY shader_visibility = D3D12_SHADER_VISIBILITY_ALL);

	void setVertexShader(VertexShader* vertex_shader);

	void setPixelShader(PixelShader* pixel_shader);

	void build();

	/* Commands ******************************************************************************/

	void CMD_bind(ID3D12GraphicsCommandList* cmd_list);

	void CMD_setShaderResourceView(uint32_t shader_register, Resource* resource);

	void CMD_setViewportSize(float width, float height);
	void CMD_setViewportSize(uint32_t width, uint32_t height);

	void CMD_setRenderTargets(std::vector<DescriptorHandle> render_targets);

	void CMD_clearRenderTarget(DescriptorHandle render_target, float red = 0, float green = 0, float blue = 0, float alpha = 0);

	void CMD_draw(uint32_t new_vertex_count, uint32_t new_instance_count = 1);
};
