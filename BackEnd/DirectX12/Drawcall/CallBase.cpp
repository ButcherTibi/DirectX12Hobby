#include "Drawcall.hpp"

#include <format>


std::string CallBase::buildRootSiganture()
{
	// Build Root Signature
	{
		D3D12_ROOT_SIGNATURE_DESC root_signature_desc = {};
		root_signature_desc.NumParameters = (uint32_t)params.size();
		root_signature_desc.pParameters = params.data();
		root_signature_desc.NumStaticSamplers = 0;
		root_signature_desc.pStaticSamplers = nullptr;
		root_signature_desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		ComPtr<ID3DBlob> blob;
		ComPtr<ID3DBlob> err_blob;

		checkDX12(D3D12SerializeRootSignature(
			&root_signature_desc,
			D3D_ROOT_SIGNATURE_VERSION_1_0,
			blob.GetAddressOf(),
			err_blob.GetAddressOf()
		));

		checkDX12(context->dev->CreateRootSignature(0,
			blob->GetBufferPointer(), blob->GetBufferSize(), IID_PPV_ARGS(root_signature.GetAddressOf())
		));
	}

	// Create Root Signature
	std::string hlsl_root_signature;
	{
		hlsl_root_signature = "[RootSignature(\"";

		for (uint32_t i = 0; i < params.size(); i++) {

			auto& param = params[i];

			switch (param.ParameterType) {
			case D3D12_ROOT_PARAMETER_TYPE_SRV: {
				hlsl_root_signature += std::format("SRV(t{})", param.Descriptor.ShaderRegister);
				break;
			}
			case D3D12_ROOT_PARAMETER_TYPE_UAV: {
				hlsl_root_signature += std::format("UAV(u{})", param.Descriptor.ShaderRegister);
				break;
			}
			default: __debugbreak();
			}

			if (i != params.size() - 1) {
				hlsl_root_signature += ", \n";
			}
		}

		hlsl_root_signature += "\")]";
	}

	return hlsl_root_signature;
}

void CallBase::setShaderResourceViewParam(uint32_t shader_register, D3D12_SHADER_VISIBILITY shader_visibility)
{
	auto& new_param = params.emplace_back();
	new_param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
	new_param.Descriptor.ShaderRegister = shader_register;
	new_param.Descriptor.RegisterSpace = 0;
	new_param.ShaderVisibility = shader_visibility;
}

void CallBase::setUnorderedAccessViewParam(uint32_t shader_register, D3D12_SHADER_VISIBILITY shader_visibility)
{
	auto& new_param = params.emplace_back();
	new_param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_UAV;
	new_param.Descriptor.ShaderRegister = shader_register;
	new_param.Descriptor.RegisterSpace = 0;
	new_param.ShaderVisibility = shader_visibility;
}
