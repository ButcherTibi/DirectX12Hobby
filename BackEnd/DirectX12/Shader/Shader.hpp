#pragma once

#include <DirectX12/Context/Context.hpp>


class Shader {
	std::vector<uint8_t> source_code;
	std::string file_path;  // used to resolving includes
	std::string target;

	ComPtr<ID3DBlob> errors = nullptr;

public:
	ComPtr<ID3DBlob> cso = nullptr;

protected:
	void loadSourceCode(std::string file_path, std::string target);
	
public:
	void compile(std::string root_signature);
};

class VertexShader : public Shader {
	void createFromSourceCodeFile(std::string file_path);
};

class PixelShader : public Shader {
	void createFromSourceCodeFile(std::string file_path);
};
