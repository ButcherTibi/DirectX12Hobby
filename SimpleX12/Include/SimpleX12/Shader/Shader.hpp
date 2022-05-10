#pragma once

#include "../Context/Context.hpp"
#include "../ButchersToolbox/Filesys/Filesys.hpp"

#undef min
#undef max


class Shader {
	using TimePoint = std::chrono::time_point<std::chrono::system_clock>;

	Context* ctx = nullptr;
	std::wstring file_path;
	std::wstring hlsl_target;  // vs_6_0, ps_6_0, cs_6_0

	ComPtr<IDxcBlob> bytecode = nullptr;

	// Shader Hot Reloading
	ComPtr<IDxcBlobUtf8> last_errors = nullptr;
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
