#include "DX12.hpp"

// Mine
#include "ButchersToolbox/Filesys/Filesys.hpp"


void Shader::createFromSourceCode(std::wstring file_path, uint16_t shader_stages)
{
	std::vector<uint8_t> source_code;

	filesys::Path path = file_path;
	path.readFile(source_code);

	uint32_t flags;
	if (HelloTriangle::is_pix_debugger_enabled) {
		flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
	}
	else {
		flags = 0;
	}

	if (shader_stages & (uint16_t)ShaderStages::VERTEX) {
		if (D3DCompile2(
			source_code.data(), source_code.size(),
			nullptr,  // file path
			nullptr,  // macros
			nullptr,  // include
			"VertexMain",  // entry point
			"vs_5_0",
			flags,
			0,  // effect flags
			0, 0, 0,
			vertex_shader_cso.GetAddressOf(), vertex_shader_errors.GetAddressOf()
		) != S_OK)
		{
			OutputDebugStringA((char*)vertex_shader_errors->GetBufferPointer());
			__debugbreak();
		}
	}

	if (shader_stages & (uint16_t)ShaderStages::PIXEL) {
		if (D3DCompile2(
			source_code.data(), source_code.size(),
			nullptr,  // file path
			nullptr,  // macros
			nullptr,  // include
			"PixelMain",  // entry point
			"ps_5_0",
			flags,
			0,  // effect flags
			0, 0, 0,
			pixel_shader_cso.GetAddressOf(), pixel_shader_errors.GetAddressOf()
		) != S_OK)
		{
			OutputDebugStringA((char*)pixel_shader_errors->GetBufferPointer());
			__debugbreak();
		}
	}
}
