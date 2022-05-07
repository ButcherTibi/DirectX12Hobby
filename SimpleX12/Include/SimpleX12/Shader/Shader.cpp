#include "Shader.hpp"


void Shader::createFromSourceCodeFile(Context* new_context, std::wstring new_file_path, std::wstring new_hlsl_target)
{
	this->ctx = new_context;
	this->file_path = new_file_path;
	this->hlsl_target = new_hlsl_target;
}

bool Shader::reload(std::string& new_root_signature)
{
	auto loadAndCompile = [&]() {

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
		{
			DxcBuffer buffer = {};
			buffer.Ptr = source_code.data();
			buffer.Size = source_code.size();

			std::vector<const wchar_t*> arguments;
			{
				arguments.push_back(L"-E");
				arguments.push_back(L"main");

				arguments.push_back(L"-T");
				arguments.push_back(hlsl_target.data());

				//arguments.push_back(L"-Qstrip_debug");
				//arguments.push_back(L"-Qstrip_reflect");

				arguments.push_back(DXC_ARG_DEBUG);
				arguments.push_back(DXC_ARG_WARNINGS_ARE_ERRORS);
				// arguments.push_back(DXC_ARG_PACK_MATRIX_ROW_MAJOR);
			}

			checkDX12(ctx->hlsl_compiler->Compile(
				&buffer,
				arguments.data(), (uint32_t)arguments.size(),
				nullptr,
				IID_PPV_ARGS(&compilation_result)
			));

			ComPtr<IDxcBlobUtf8> errors;
			checkDX12(compilation_result->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&errors), nullptr));

			if (errors->GetStringLength() > 0) {
				OutputDebugStringA(errors->GetStringPointer());
				__debugbreak();
			}

			checkDX12(compilation_result->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&bytecode), nullptr));
		}
		
		root_signature = new_root_signature;
		last_check = std::chrono::system_clock::now();
		modified_time = filesys::File<>::lastModifiedTime(file_path);
	};

	if (source_code.size() == 0) {
		loadAndCompile();
		return true;
	}
	else if(root_signature != new_root_signature) {
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
