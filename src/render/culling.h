#ifndef CULLING_H
#define CULLING_H

#include "../util/util.h"

struct Plane {
	glm::vec3 normal;
	float dist;
};

void culling_update_planes(const glm::mat4 &pv);
bool culling_chunk_visible(glm::ivec3 &pos);

#endif
