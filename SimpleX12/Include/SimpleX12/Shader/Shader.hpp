#pragma once

#include "../Context/Context.hpp"
#include "../ButchersToolbox/Filesys/Filesys.hpp"

#undef min
#undef max


class Shader {
	using TimePoint = std::chrono::time_point<std::chrono::system_clock>;

	Context* ctx = nullptr;
	std::wstring file_path;  // used to resolving includes
	std::wstring hlsl_target;

	std::string source_code;
	std::string root_signature;

	ComPtr<IDxcResult> compilation_result;
	ComPtr<IDxcBlob> bytecode;

	// Shader Hot Reloading
	TimePoint last_check = TimePoint::min();
	TimePoint modified_time = TimePoint::min();

protected:
	void createFromSourceCodeFile(Context* context, std::wstring file_path, std::wstring hlsl_target);
	
public:
	bool reload(std::string& root_signature);

	D3D12_SHADER_BYTECODE getByteCode();
};


class VertexShader : public Shader {
public:
	void createFromSourceCodeFile(Context* context, std::wstring file_path);
};

class PixelShader : public Shader {
public:
	void createFromSourceCodeFile(Context* context, std::wstring file_path);
};

class ComputeShader : public Shader {
public:
	void createFromSourceCodeFile(Context* context, std::wstring file_path);
};
