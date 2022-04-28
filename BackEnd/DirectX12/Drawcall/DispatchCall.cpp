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

void DispatchCall::build()
{
	// Create Root Signature
	{
		std::string hlsl_root_signature = CallBase::buildRootSiganture();
		shader->compile(hlsl_root_signature);
	}

	// Pipeline
	{
		pipe_desc.pRootSignature = root_signature.Get();
		pipe_desc.CS.pShaderBytecode = shader->cso->GetBufferPointer();
		pipe_desc.CS.BytecodeLength = shader->cso->GetBufferSize();

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

void DispatchCall::CMD_setShaderResourceView(uint32_t shader_register, Resource* resource)
{
	for (uint32_t i = 0; i < params.size(); i++) {
		auto& param = params[i];

		if (param.ParameterType == D3D12_ROOT_PARAMETER_TYPE_SRV &&
			param.Descriptor.ShaderRegister == shader_register)
		{
			context->cmd_list->SetComputeRootShaderResourceView(i, resource->gpu_adress());
			return;
		}
	}

	__debugbreak();
}

void DispatchCall::CMD_setUnorderedAccessView(uint32_t shader_register, Resource* resource)
{
	for (uint32_t i = 0; i < params.size(); i++) {
		auto& param = params[i];

		if (param.ParameterType == D3D12_ROOT_PARAMETER_TYPE_UAV &&
			param.Descriptor.ShaderRegister == shader_register)
		{
			context->cmd_list->SetComputeRootUnorderedAccessView(i, resource->gpu_adress());
			return;
		}
	}

	__debugbreak();
}

void DispatchCall::CMD_dispatch(uint32_t thread_group_count_x, uint32_t thread_group_count_y, uint32_t thread_group_count_z)
{
	context->cmd_list->Dispatch(thread_group_count_x, thread_group_count_y, thread_group_count_z);
}
