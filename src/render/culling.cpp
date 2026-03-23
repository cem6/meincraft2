#include "culling.h"

Plane planes[6];

void culling_update_planes(const glm::mat4 &pv) {
	// left
    planes[0].normal = glm::vec3(pv[0][3] + pv[0][0], pv[1][3] + pv[1][0], pv[2][3] + pv[2][0]);
    planes[0].dist   = pv[3][3] + pv[3][0];

    // right
    planes[1].normal = glm::vec3(pv[0][3] - pv[0][0], pv[1][3] - pv[1][0], pv[2][3] - pv[2][0]);
    planes[1].dist   = pv[3][3] - pv[3][0];

    // bottom
    planes[2].normal = glm::vec3(pv[0][3] + pv[0][1], pv[1][3] + pv[1][1], pv[2][3] + pv[2][1]);
    planes[2].dist   = pv[3][3] + pv[3][1];

    // top
    planes[3].normal = glm::vec3(pv[0][3] - pv[0][1], pv[1][3] - pv[1][1], pv[2][3] - pv[2][1]);
    planes[3].dist   = pv[3][3] - pv[3][1];

    // front
    planes[4].normal = glm::vec3(pv[0][3] + pv[0][2], pv[1][3] + pv[1][2], pv[2][3] + pv[2][2]);
    planes[4].dist   = pv[3][3] + pv[3][2];

    // back
    planes[5].normal = glm::vec3(pv[0][3] - pv[0][2], pv[1][3] - pv[1][2], pv[2][3] - pv[2][2]);
    planes[5].dist   = pv[3][3] - pv[3][2];
	
	// normalize
	for (int i = 0; i < 6; i++) {
		float len = glm::length(planes[i].normal);
		planes[i].normal /= len;
		planes[i].dist /= len;
	}
}

float _signed_distance(int i, glm::vec3 &p) {
	return glm::dot(planes[i].normal, p) + planes[i].dist;
}

bool culling_chunk_visible(glm::ivec3 &pos) {
	glm::vec3 min = glm::vec3(pos.x * CHUNK_SIZE_X, pos.y * CHUNK_SIZE_Y, pos.z * CHUNK_SIZE_Z);
	glm::vec3 max = min + glm::vec3(CHUNK_SIZE_X, CHUNK_SIZE_Y, CHUNK_SIZE_Z);

	for (int i = 0; i < 6; i++) {
		glm::vec3 p = min;
		if (planes[i].normal.x >= 0) p.x = max.x;
		if (planes[i].normal.y >= 0) p.y = max.y;
		if (planes[i].normal.z >= 0) p.z = max.z;

		if (_signed_distance(i, p) < 0) return false;
	}
	return true;
}
