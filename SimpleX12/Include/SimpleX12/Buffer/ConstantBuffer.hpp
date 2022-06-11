#pragma once

#include "../Resource/Resource.hpp"
#include "DirectXMath.h"


struct ConstantBufferField {
	enum class FieldType {
		UINT,
		INT,

		FLOAT,
		FLOAT2,
		FLOAT4,
		FLOAT_ARR,
		matrix
	};

	FieldType type;
	size_t offset;
};


// Designed to wrap usage and declaration of constant buffer.
// Allows creating the layout of the constant buffer one field at a time.
// With this calculating that DISGUSTING alignment is no longer necesary.
class ConstantBuffer : public Buffer {
	size_t total_offset;
	std::vector<ConstantBufferField> fields;

private:
	void _ensureCreateAndMapped();

public:
	void create(Context* context);


	// each method adds a field to the buffer structure, calculating the offset required
	// the order of addition must match with the field_idx
	// Example:
	//   addUint();
	//   addFloat();
	//
	//   setUint(1, 1234);  // bad uint was added first
	//   setFloat(0, 1234.f);

	void _add4BytesField(ConstantBufferField::FieldType type);
	void addUint();
	void addInt();
	void addFloat();

	void addFloat2();
	void addFloat4();
	void addFloat4Array(uint32_t array_size);
	void addMatrix();

	void _set4BytesField(uint32_t field_idx, void* data, ConstantBufferField::FieldType type);
	void setUint(uint32_t field_idx, uint32_t value);
	void setInt(uint32_t field_idx, int32_t value);
	void setFloat(uint32_t field_idx, float value);

	void setFloat2(uint32_t field_idx, DirectX::XMFLOAT2& value);
	void setFloat4(uint32_t field_idx, float x, float y = 0, float z = 0, float w = 0);
	void setFloat4Array(uint32_t field_idx, uint32_t array_idx, DirectX::XMFLOAT4& value);
	void setFloat4Array(uint32_t field_idx, uint32_t array_idx, float x, float y = 0, float z = 0, float w = 0);
	void setMatrix(uint32_t field_idx, DirectX::XMMATRIX& matrix);
};
