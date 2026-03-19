#ifndef CAMERA_H
#define CAMERA_H

#include "../util/util.h"

struct Camera {
	glm::vec3 pos 	= glm::vec3(-20.0f, 2.0f, 5.0f);
	glm::vec3 front = glm::vec3(1.0f, 0.0f, 0.0f);	// camera direction
	glm::vec3 up 	= glm::vec3(0.0f, 1.0f, 0.0f);	// up direction
	float yaw = 0.0f; // -90 ?
	float pitch = 0.0f;
};
extern Camera camera;

void camera_update_rotation(float xoffset, float yoffset);
void camera_update_position(float deltaTime);

#endif
