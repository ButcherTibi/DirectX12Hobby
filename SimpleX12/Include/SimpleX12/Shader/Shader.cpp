#include "Shader.hpp"


void Shader::createFromSourceCodeFile(Context* new_context, std::wstring new_file_path, std::wstring new_hlsl_target)
{
	this->ctx = new_context;
	this->file_path = new_file_path;
	this->hlsl_target = new_hlsl_target;
}

bool areErrorsSame(ComPtr<IDxcBlobUtf8>& last_error, ComPtr<IDxcBlobUtf8>& new_error)
{
	if (last_error->GetStringLength() != new_error->GetStringLength()) {
		return false;
	}

	for (uint32_t i = 0; i < new_error->GetStringLength(); i++) {

		if (last_error->GetStringPointer() + i != new_error->GetStringPointer() + i) {
			return false;
		}
	}

	return true;
}

/*
  In case of separate PDB for shader

  How is this better ? More verbose API instead of a single function, more files and less documented.
  This is software decadence, it's not optimized for life.

  Links:
  https://www.youtube.com/watch?v=tyyKeTsdtmo&t=879s
  https://simoncoenen.com/blog/programming/graphics/DxcCompiling

  arguments.push_back(L"-Fd");  // write debug information
  arguments.push_back(pdb_dir.data());
  arguments.push_back(DXC_ARG_DEBUG_NAME_FOR_BINARY);
  
  ComPtr<IDxcBlob> pdb;
  ComPtr<IDxcBlobUtf16> pdb_hash;
  checkDX12(compilation_result->GetOutput(DXC_OUT_PDB, IID_PPV_ARGS(&pdb), &pdb_hash));
 
  pdb_dir.append(pdb_hash->GetStringPointer());
  filesys::File<>::write(pdb_dir, pdb->GetBufferSize(), pdb->GetBufferPointer());
*/


bool Shader::reload(std::string& root_signature)
{
	auto loadAndCompile = [&]() {

		std::string source_code;

		// Read source code
		filesys::File<>::read(file_path, source_code);

		// Add Root Signature
		{
			auto pos = source_code.find("//RootSignature");
			if (pos != std::string::npos) {
				source_code.replace(source_code.begin() + pos, source_code.begin() + pos + 16, root_signature);
			}
		}

		// Compile
		ComPtr<IDxcResult> compilation_result;
		{
			DxcBuffer buffer = {};
			buffer.Ptr = source_code.data();
			buffer.Size = source_code.size();

			std::vector<const wchar_t*> arguments;
			{
				// Entry Point
				arguments.push_back(L"-E");
				arguments.push_back(L"main");

				// Target
				arguments.push_back(L"-T");
				arguments.push_back(hlsl_target.data());

				// Generate Symbols
				arguments.push_back(DXC_ARG_DEBUG);  // generate pdb

				// don't give a fuck about having debug symbols inside the shader because we are developing
				arguments.push_back(L"-Qembed_debug");  

				// Code settings
				arguments.push_back(DXC_ARG_WARNINGS_ARE_ERRORS);
			}

			checkDX12(ctx->hlsl_compiler->Compile(
				&buffer,
				arguments.data(), (uint32_t)arguments.size(),
				nullptr,
				IID_PPV_ARGS(&compilation_result)
			));
		}

		ComPtr<IDxcBlobUtf8> new_errors;
		checkDX12(compilation_result->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&new_errors), nullptr));

		if (new_errors->GetStringLength() == 0) {

			checkDX12(compilation_result->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&bytecode), nullptr));
		}
		else if (last_errors == nullptr || areErrorsSame(last_errors, new_errors) == false) {

			OutputDebugStringA(new_errors->GetStringPointer());
			new_errors.CopyTo(&last_errors);
		}

		last_check = std::chrono::system_clock::now();
		modified_time = filesys::File<>::lastModifiedTime(file_path);
	};

	if (bytecode == nullptr) {
		loadAndCompile();
		return true;
	}
	else if (last_check + std::chrono::seconds(1) < std::chrono::system_clock::now() &&
		modified_time < filesys::File<>::lastModifiedTime(file_path))
	{
		loadAndCompile();
		return true;
	}

	return false;
}

D3D12_SHADER_BYTECODE Shader::getByteCode()
{
	D3D12_SHADER_BYTECODE r = {};
	r.BytecodeLength = bytecode->GetBufferSize();
	r.pShaderBytecode = bytecode->GetBufferPointer();
	return r;
}

void VertexShader::createFromSourceCodeFile(Context* new_context, std::wstring new_file_path)
{
	Shader::createFromSourceCodeFile(new_context, new_file_path, L"vs_6_0");
}

void PixelShader::createFromSourceCodeFile(Context* new_context, std::wstring new_file_path)
{
	Shader::createFromSourceCodeFile(new_context, new_file_path, L"ps_6_0");
}

void ComputeShader::createFromSourceCodeFile(Context* new_context, std::wstring new_file_path)
{
	Shader::createFromSourceCodeFile(new_context, new_file_path, L"cs_6_0");
}
