#include "Drawcall.hpp"


void Drawcall::CMD_bind(ID3D12GraphicsCommandList* new_cmd_list)
{
	if (cmd_list != nullptr) {
		__debugbreak();
	}

	this->cmd_list = new_cmd_list;

	cmd_list->SetGraphicsRootSignature(root_signature.Get());
	cmd_list->SetPipelineState(pipeline.Get());
}

void Drawcall::CMD_setShaderResourceView(uint32_t shader_register, Resource* resource)
{
	assertCmdListSet();

	for (uint32_t i = 0; i < params.size(); i++) {
		auto& param = params[i];

		if (param.ParameterType == D3D12_ROOT_PARAMETER_TYPE_SRV &&
			param.Descriptor.ShaderRegister == shader_register)
		{
			cmd_list->SetGraphicsRootShaderResourceView(i, resource->gpu_adress());
			return;
		}
	}

	__debugbreak();
}

void Drawcall::CMD_setViewportSize(float width, float height)
{
	assertCmdListSet();

	viewport.Width = width;
	viewport.Height = height;
}

void Drawcall::CMD_setViewportSize(uint32_t width, uint32_t height)
{
	CMD_setViewportSize((float)width, (float)height);
}

void Drawcall::CMD_setRenderTargets(std::vector<DescriptorHandle> render_targets)
{
	assertCmdListSet();

	std::array<D3D12_CPU_DESCRIPTOR_HANDLE, 8> handles;

	for (uint32_t i = 0; i < render_targets.size(); i++) {
		handles[i] = render_targets[i].cpu_handle;
	}

	cmd_list->OMSetRenderTargets((uint32_t)render_targets.size(), handles.data(), false, nullptr);
}

void Drawcall::CMD_clearRenderTarget(DescriptorHandle render_target, float red, float green, float blue, float alpha)
{
	assertCmdListSet();

	const float clearColor[] = { red, green, blue, alpha };
	cmd_list->ClearRenderTargetView(render_target.cpu_handle, clearColor, 0, nullptr);
}

void Drawcall::CMD_draw(uint32_t vertex_count, uint32_t instance_count)
{
	assertCmdListSet();

	// Input Assembly
	cmd_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Viewport
	cmd_list->RSSetViewports(1, &viewport);

	// Scissors
	{
		if (scissor.right == 0xFFFF'FFFF) {
			scissor.right = (uint32_t)viewport.Width;
			scissor.bottom = (uint32_t)viewport.Height;
		}

		cmd_list->RSSetScissorRects(1, &scissor);
	}

	cmd_list->DrawInstanced(vertex_count, instance_count, 0, 0);
}