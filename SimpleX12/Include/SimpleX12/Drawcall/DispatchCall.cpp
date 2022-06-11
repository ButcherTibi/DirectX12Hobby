#include "Drawcall.hpp"


void DispatchCall::create(Context* new_context)
{
	context = new_context;

	// Pipeline
	{
		pipe_desc = {};
	}
}

void DispatchCall::setComputeShader(ComputeShader* compute_shader)
{
	shader = compute_shader;
}

void DispatchCall::rebuild()
{
	// Create Root Signature
	if (hlsl_root_signature.size() == 0) {
		hlsl_root_signature = CallBase::buildRootSiganture();
		;
	}

	bool recreate_pipeline = false;

	if (shader->reload(hlsl_root_signature)) {
		recreate_pipeline = true;
	}

	// Pipeline
	if (recreate_pipeline) {
		pipe_desc.pRootSignature = root_signature.Get();
		pipe_desc.CS = shader->getByteCode();

		checkDX12(
			context->dev->CreateComputePipelineState(&pipe_desc, IID_PPV_ARGS(&pipeline))
		);
	}
}

void DispatchCall::CMD_bind()
{
	context->cmd_list->SetComputeRootSignature(root_signature.Get());
	context->cmd_list->SetPipelineState(pipeline.Get());
}

void DispatchCall::CMD_setBufferParam(uint32_t shader_register, Buffer& resource)
{
	for (uint32_t i = 0; i < params.size(); i++) {
		auto& param = params[i];

		if (param.type == ShaderInputType::Buffer &&
			param.shader_register == shader_register)
		{
			context->cmd_list->SetComputeRootShaderResourceView(i, resource.gpu_adress());
			return;
		}
	}

	__debugbreak();
}

void DispatchCall::CMD_setUnorderedAccessResourceParam(uint32_t shader_register, Resource& resource)
{
	for (uint32_t i = 0; i < params.size(); i++) {
		auto& param = params[i];

		if (param.type == ShaderInputType::UnorderedAccessResource &&
			param.shader_register == shader_register)
		{
			context->cmd_list->SetComputeRootUnorderedAccessView(i, resource.gpu_adress());
			return;
		}
	}

	__debugbreak();
}

void DispatchCall::CMD_dispatch(uint32_t thread_group_count_x, uint32_t thread_group_count_y, uint32_t thread_group_count_z)
{
	context->cmd_list->Dispatch(thread_group_count_x, thread_group_count_y, thread_group_count_z);
}
