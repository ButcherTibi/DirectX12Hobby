#include <DirectX12/Shader/Shader.hpp>

// Mine
#include <ButchersToolbox/Filesys/Filesys.hpp>


void Shader::loadSourceCode(std::string new_file_path, std::string target)
{
	filesys::Path path = new_file_path;
	path.readFile(source_code);

	this->file_path = new_file_path;
	this->target = target;
}

void Shader::compile(std::string root_signature)
{
	uint32_t flags;
	if (Context::is_pix_debugger_enabled) {
		flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
	}
	else {
		flags = 0;
	}

	std::array<D3D_SHADER_MACRO, 2> macros;
	macros[0].Name = "RootSign";
	macros[0].Definition = root_signature.c_str();

	macros[macros.size() - 1] = { nullptr, nullptr };

	if (D3DCompile2(
		source_code.data(), source_code.size(),
		file_path.c_str(),  // file path
		macros.data(),  // macros
		D3D_COMPILE_STANDARD_FILE_INCLUDE,  // include
		"main",  // entry point
		target.c_str(),  // target
		flags,
		0,  // effect flags
		0, 0, 0,
		cso.GetAddressOf(), errors.GetAddressOf()
	) != S_OK)
	{
		OutputDebugStringA((char*)errors->GetBufferPointer());
		__debugbreak();
	}
}

void VertexShader::createFromSourceCodeFile(std::string file_path)
{
	loadSourceCode(file_path, "vs_5_0");
}

void PixelShader::createFromSourceCodeFile(std::string file_path)
{
	loadSourceCode(file_path, "ps_5_0");
}
