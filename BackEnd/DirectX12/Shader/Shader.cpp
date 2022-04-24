#include <DirectX12/Shader/Shader.hpp>

// Mine
#include <ButchersToolbox/Filesys/Filesys.hpp>


void Shader::loadSourceCode(std::string new_file_path, std::string new_target)
{
	filesys::Path path = new_file_path;
	path.readFile(source_code);

	this->file_path = new_file_path;
	this->target = new_target;
}

void Shader::compile(std::string root_signature)
{
	auto pos = source_code.find("//RootSignature");
	if (pos != std::string::npos) {
		source_code.replace(source_code.begin() + pos, source_code.begin() + pos + 16, root_signature);
	}

	uint32_t flags;
	if (Context::is_pix_debugger_enabled) {
		flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
	}
	else {
		flags = 0;
	}

	if (D3DCompile2(
		source_code.data(), source_code.size(),
		file_path.c_str(),  // file path
		nullptr,  // macros
		D3D_COMPILE_STANDARD_FILE_INCLUDE,  // include
		"main",  // entry point
		target.c_str(),  // target
		flags,
		0,  // effect flags
		0, 0, 0,
		cso.GetAddressOf(), errors.GetAddressOf()) != S_OK)
	{
		OutputDebugStringA((char*)errors->GetBufferPointer());
		__debugbreak();
	}
}

void VertexShader::createFromSourceCodeFile(std::string new_file_path)
{
	loadSourceCode(new_file_path, "vs_5_0");
}

void PixelShader::createFromSourceCodeFile(std::string new_file_path)
{
	loadSourceCode(new_file_path, "ps_5_0");
}
