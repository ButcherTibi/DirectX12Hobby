#include "./SculptMesh.hpp"

using namespace scme;

InstanceIndex SculptMesh::addInstance()
{
	scme::InstanceIndex idx;
	instances.emplace(idx);

	markInstanceUpdate(idx);
	return idx;
}
