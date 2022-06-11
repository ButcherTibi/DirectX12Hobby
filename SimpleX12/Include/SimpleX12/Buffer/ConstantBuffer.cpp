#include "./ConstantBuffer.hpp"
#include "../ButchersToolbox/DebugUtils.hpp"


void ConstantBuffer::_ensureCreateAndMapped()
{
	// ensure buffer is create
	if (resource == nullptr) {

		// round up, must be a multiple of 16
		resizeDiscard(total_offset + 16 - total_offset % 16);
	}

	// ensure buffer is mapped
	if (mapped_mem == nullptr) {
		mapNoRead();
	}
}

void ConstantBuffer::create(Context* new_context)
{
	context = new_context;

	heap_props.Type = D3D12_HEAP_TYPE_UPLOAD;
	heap_props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heap_props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

	desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Width = 0;
	desc.Height = 1;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	desc.Flags = D3D12_RESOURCE_FLAG_NONE;

	states = D3D12_RESOURCE_STATE_GENERIC_READ;

	total_offset = 0;
}

void ConstantBuffer::_add4BytesField(ConstantBufferField::FieldType type)
{
	ConstantBufferField& new_field = this->fields.emplace_back();
	new_field.type = type;
	new_field.offset = total_offset;

	total_offset = new_field.offset + 4;
}

void ConstantBuffer::addUint()
{
	_add4BytesField(ConstantBufferField::FieldType::UINT);
}

void ConstantBuffer::addInt()
{
	_add4BytesField(ConstantBufferField::FieldType::INT);
}

void ConstantBuffer::addFloat()
{
	_add4BytesField(ConstantBufferField::FieldType::FLOAT);
}

void ConstantBuffer::addFloat2()
{
	ConstantBufferField& new_field = this->fields.emplace_back();
	new_field.type = ConstantBufferField::FieldType::FLOAT2;

	size_t remainder = total_offset % 16;

	if (remainder >= 8) {
		new_field.offset = total_offset;
	}
	else {
		new_field.offset = total_offset + remainder;
	}

	total_offset = new_field.offset + sizeof(DirectX::XMFLOAT2);
}

void ConstantBuffer::addFloat4()
{
	ConstantBufferField& new_field = this->fields.emplace_back();
	new_field.type = ConstantBufferField::FieldType::FLOAT4;

	size_t remainder = total_offset % 16;

	if (remainder != 0) {
		new_field.offset = total_offset + remainder;
	}
	else {
		new_field.offset = total_offset;
	}

	total_offset = new_field.offset + sizeof(DirectX::XMFLOAT4);
}

void ConstantBuffer::addFloat4Array(uint32_t array_size)
{
	ConstantBufferField& new_field = this->fields.emplace_back();
	new_field.type = ConstantBufferField::FieldType::FLOAT_ARR;

	size_t padding = total_offset % 16;

	// arrays elements are always 16 bytes wide, so round up
	new_field.offset = total_offset + padding;

	total_offset = new_field.offset + sizeof(DirectX::XMFLOAT4) * array_size;
}

void ConstantBuffer::addMatrix()
{
	ConstantBufferField& new_field = fields.emplace_back();
	new_field.type = ConstantBufferField::FieldType::matrix;

	size_t padding = total_offset % 16;
	new_field.offset = total_offset + padding;

	total_offset = new_field.offset + sizeof(DirectX::XMMATRIX);
}

void ConstantBuffer::_set4BytesField(uint32_t field_idx, void* data, ConstantBufferField::FieldType type)
{
	assert_cond(fields[field_idx].type == type,
		"wrong field index, field type mismatch");

	_ensureCreateAndMapped();

	uint8_t* mem = ((uint8_t*)mapped_mem) + fields[field_idx].offset;
	std::memcpy(mem, data, 4);
}

void ConstantBuffer::setUint(uint32_t field_idx, uint32_t value)
{
	_set4BytesField(field_idx, &value, ConstantBufferField::FieldType::UINT);
}

void ConstantBuffer::setInt(uint32_t field_idx, int32_t value)
{
	_set4BytesField(field_idx, &value, ConstantBufferField::FieldType::INT);
}

void ConstantBuffer::setFloat(uint32_t field_idx, float value)
{
	_set4BytesField(field_idx, &value, ConstantBufferField::FieldType::FLOAT);
}

void ConstantBuffer::setFloat2(uint32_t field_idx, DirectX::XMFLOAT2& value)
{
	assert_cond(fields[field_idx].type == ConstantBufferField::FieldType::FLOAT2,
		"wrong field index, field type mismatch");

	_ensureCreateAndMapped();

	uint8_t* mem = ((uint8_t*)mapped_mem) + fields[field_idx].offset;
	std::memcpy(mem, &value, sizeof(DirectX::XMFLOAT2));
}

void ConstantBuffer::setFloat4(uint32_t field_idx, float x, float y, float z, float w)
{
	assert_cond(fields[field_idx].type == ConstantBufferField::FieldType::FLOAT4,
		"wrong field index, field type mismatch");

	_ensureCreateAndMapped();

	uint8_t* mem = ((uint8_t*)mapped_mem) + fields[field_idx].offset;
	std::memcpy(mem, &x, 4);
	std::memcpy(mem + 4, &y, 4);
	std::memcpy(mem + 8, &z, 4);
	std::memcpy(mem + 12, &w, 4);
}

void ConstantBuffer::setFloat4Array(uint32_t field_idx, uint32_t array_idx, DirectX::XMFLOAT4& value)
{
	assert_cond(fields[field_idx].type == ConstantBufferField::FieldType::FLOAT_ARR,
		"wrong field index, field type mismatch");

	_ensureCreateAndMapped();

	uint8_t* mem = ((uint8_t*)mapped_mem) + fields[field_idx].offset + 16 * array_idx;
	std::memcpy(mem, &value, sizeof(DirectX::XMFLOAT4));
}

void ConstantBuffer::setFloat4Array(uint32_t field_idx, uint32_t array_idx, float x, float y, float z, float w)
{
	assert_cond(fields[field_idx].type == ConstantBufferField::FieldType::FLOAT_ARR,
		"wrong field index, field type mismatch");

	_ensureCreateAndMapped();

	uint8_t* mem = ((uint8_t*)mapped_mem) + fields[field_idx].offset + 16 * array_idx;
	std::memcpy(mem, &x, 4);
	std::memcpy(mem + 4, &y, 4);
	std::memcpy(mem + 8, &z, 4);
	std::memcpy(mem + 12, &w, 4);
}

void ConstantBuffer::setMatrix(uint32_t field_idx, DirectX::XMMATRIX& matrix)
{
	assert_cond(fields[field_idx].type == ConstantBufferField::FieldType::matrix,
		"wrong field index, field type mismatch");

	_ensureCreateAndMapped();

	uint8_t* mem = ((uint8_t*)mapped_mem) + fields[field_idx].offset;
	std::memcpy(mem, &matrix, sizeof(DirectX::XMMATRIX));
}