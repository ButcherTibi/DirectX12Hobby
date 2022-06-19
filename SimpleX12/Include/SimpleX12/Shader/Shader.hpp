#pragma once

#include <string>
#include <chrono>

// Windows
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

#include <d3d12.h>

// HLSL Compiler
#include <dxcapi.h>

#undef min
#undef max

class Context;


class Shader {
	using TimePoint = std::chrono::time_point<std::chrono::system_clock>;

	Context* ctx = nullptr;
	std::wstring file_path;
	std::wstring shaders_folder_path;
	std::wstring hlsl_target;  // vs_6_0, ps_6_0, cs_6_0

	ComPtr<IDxcBlob> bytecode = nullptr;

	// Shader Hot Reloading
	ComPtr<IDxcBlobUtf8> last_errors = nullptr;
	TimePoint last_check = TimePoint::min();
	TimePoint modified_time = TimePoint::min();

private:
	void loadAndCompile(std::string& root_signature);

protected:
	// Since DXC is based on LLVM it does NOT accept filesystem paths with spaces.
	// It does not work on the command line.
	// It won't work even if you supply your own include handler.
	// Will give out strange infinite recursion error (include path nested too deply).
	// Using any relative includes in HLSL will throw a hlsl::Exception that does
	// nothing, the compilation will be succesfull with no errors.
	void createFromSourceCodeFile(Context* context,
		std::wstring file_path, std::wstring shaders_folder_path, std::wstring hlsl_target);
	
public:
	bool reload(std::string& root_signature);

	D3D12_SHADER_BYTECODE getByteCode();
};


class VertexShader : public Shader {
public:
	void createFromSourceCodeFile(Context* context,
		std::wstring file_path, std::wstring shaders_folder_path = L"");
};

class PixelShader : public Shader {
public:
	void createFromSourceCodeFile(Context* context,
		std::wstring file_path, std::wstring shaders_folder_path = L"");
};

class ComputeShader : public Shader {
public:
	void createFromSourceCodeFile(Context* context,
		std::wstring file_path, std::wstring shaders_folder_path = L"");
};
