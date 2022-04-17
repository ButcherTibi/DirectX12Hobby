// Header
#include "GPU_ShaderTypes.hpp"


DirectX::XMFLOAT3 dxConvert(glm::vec3& value)
{
	return { value.x, value.y, value.z };
}

DirectX::XMFLOAT3 dxConvert(float x, float y, float z)
{
	return { x, y, z };
}

DirectX::XMFLOAT4 dxConvert(glm::vec4& value)
{
	return { value.x, value.y, value.z, value.w };
}

DirectX::XMFLOAT4 dxConvert(glm::quat& value)
{
	return { value.x, value.y, value.z, value.w };
}

DirectX::XMFLOAT4X4 dxConvert(glm::mat4& val)
{
	DirectX::XMFLOAT4X4 r;
	r.m[0][0] = val[0][0];
	r.m[0][1] = val[0][1];
	r.m[0][2] = val[0][2];
	r.m[0][3] = val[0][3];

	r.m[1][0] = val[1][0];
	r.m[1][1] = val[1][1];
	r.m[1][2] = val[1][2];
	r.m[1][3] = val[1][3];

	r.m[2][0] = val[2][0];
	r.m[2][1] = val[2][1];
	r.m[2][2] = val[2][2];
	r.m[2][3] = val[2][3];

	r.m[3][0] = val[3][0];
	r.m[3][1] = val[3][1];
	r.m[3][2] = val[3][2];
	r.m[3][3] = val[3][3];

	return r;
}

DirectX::XMMATRIX dxConvertMatrix(glm::mat4& val)
{
	DirectX::XMMATRIX mat;
	mat.r[0].m128_f32[0] = val[0][0];
	mat.r[0].m128_f32[1] = val[0][1];
	mat.r[0].m128_f32[2] = val[0][2];
	mat.r[0].m128_f32[3] = val[0][3];

	mat.r[1].m128_f32[0] = val[1][0];
	mat.r[1].m128_f32[1] = val[1][1];
	mat.r[1].m128_f32[2] = val[1][2];
	mat.r[1].m128_f32[3] = val[1][3];

	mat.r[2].m128_f32[0] = val[2][0];
	mat.r[2].m128_f32[1] = val[2][1];
	mat.r[2].m128_f32[2] = val[2][2];
	mat.r[2].m128_f32[3] = val[2][3];

	mat.r[3].m128_f32[0] = val[3][0];
	mat.r[3].m128_f32[1] = val[3][1];
	mat.r[3].m128_f32[2] = val[3][2];
	mat.r[3].m128_f32[3] = val[3][3];

	return mat;
}
