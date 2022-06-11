#include "Drawcall.hpp"

#include <format>


std::string CallBase::buildRootSiganture()
{
	// Build Root Signature
	{
		std::vector<D3D12_DESCRIPTOR_RANGE> ranges;

		std::vector<D3D12_ROOT_PARAMETER> root_params;
		root_params.resize(params.size());

		for (uint32_t i = 0; i < params.size(); i++) {

			auto& param = params[i];
			auto& root_param = root_params[i];

			switch (param.type) {
			case ShaderInputType::ConstantBuffer: {
				root_param = {};
				root_param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
				root_param.Descriptor.ShaderRegister = param.shader_register;
				root_param.Descriptor.RegisterSpace = 0;
				root_param.ShaderVisibility = param.shader_visibility;
				break;
			}
			case ShaderInputType::Buffer: {
				root_param = {};
				root_param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
				root_param.Descriptor.ShaderRegister = param.shader_register;
				root_param.Descriptor.RegisterSpace = 0;
				root_param.ShaderVisibility = param.shader_visibility;
				break;
			}
			case ShaderInputType::Texture: {
				auto& range = ranges.emplace_back();
				range = {};
				range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
				range.NumDescriptors = 1;
				range.BaseShaderRegister = param.shader_register;
				range.RegisterSpace = 0;
				range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

				root_param = {};
				root_param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
				root_param.DescriptorTable.NumDescriptorRanges = 1;
				root_param.DescriptorTable.pDescriptorRanges = &range;
				root_param.ShaderVisibility = param.shader_visibility;
				break;
			}
			case ShaderInputType::UnorderedAccessResource: {
				root_param = {};
				root_param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_UAV;
				root_param.Descriptor.ShaderRegister = param.shader_register;
				root_param.Descriptor.RegisterSpace = 0;
				root_param.ShaderVisibility = param.shader_visibility;
				break;
			}
			default: __debugbreak();
			}
		}

		D3D12_ROOT_SIGNATURE_DESC root_signature_desc = {};
		root_signature_desc.NumParameters = (uint32_t)root_params.size();
		root_signature_desc.pParameters = root_params.data();
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
	{
		hlsl_root_signature = "[RootSignature(\n";

		for (uint32_t i = 0; i < params.size(); i++) {

			auto& param = params[i];

			switch (param.type) {
			case ShaderInputType::ConstantBuffer: {
				hlsl_root_signature += std::format(
					"\"  CBV(b{})", param.shader_register);
				break;
			}
			case ShaderInputType::Buffer: {
				hlsl_root_signature += std::format(
					"\"  SRV(t{})", param.shader_register);
				break;
			}
			case ShaderInputType::Texture: {
				hlsl_root_signature += std::format(
					"\"  DescriptorTable(SRV(t{}))", param.shader_register);
				break;
			}
			case ShaderInputType::UnorderedAccessResource: {
				hlsl_root_signature += std::format(
					"\"  UAV(u{})", param.shader_register);
				break;
			}
			default: __debugbreak();
			}

			if (i < params.size() - 1) {
				hlsl_root_signature += ",\"\n";
			}
			else {
				hlsl_root_signature += "\"\n";
			}
		}

		hlsl_root_signature += "\n)]";
	}

	return hlsl_root_signature;
}

void CallBase::setConstantBufferParam(uint32_t shader_register, D3D12_SHADER_VISIBILITY shader_visibility)
{
	auto& new_param = params.emplace_back();
	new_param.type = ShaderInputType::ConstantBuffer;
	new_param.shader_register = shader_register;
	new_param.shader_visibility = shader_visibility;
}

void CallBase::setBufferParam(uint32_t shader_register, D3D12_SHADER_VISIBILITY shader_visibility)
{
	auto& new_param = params.emplace_back();
	new_param.type = ShaderInputType::Buffer;
	new_param.shader_register = shader_register;
	new_param.shader_visibility = shader_visibility;
}

void CallBase::setTextureParam(uint32_t shader_register, D3D12_SHADER_VISIBILITY shader_visibility)
{
	auto& new_param = params.emplace_back();
	new_param.type = ShaderInputType::Texture;
	new_param.shader_register = shader_register;
	new_param.shader_visibility = shader_visibility;
}

void CallBase::setUnorderedAccessResourceParam(uint32_t shader_register, D3D12_SHADER_VISIBILITY shader_visibility)
{
	auto& new_param = params.emplace_back();
	new_param.type = ShaderInputType::UnorderedAccessResource;
	new_param.shader_register = shader_register;
	new_param.shader_visibility = shader_visibility;
}
