#ifndef CAMERA_H
#define CAMERA_H

#include "../util/util.h"
#include "../world/world.h"

struct Camera {
	glm::vec3 pos 	= glm::vec3(0.0f, 75.0f, 0.0f);
	glm::vec3 front = glm::vec3(1.0f, 0.0f, 0.0f);	// initial camera direction
	glm::vec3 up 	= glm::vec3(0.0f, 1.0f, 0.0f);	// up direction
	float yaw = 0.0f; // x
	float pitch = 0.0f; // y
	bool view_2d = false; // 2d
	bool physics_enabled = false;
	
	// kann hier drin bleiben ist egal
	void toggle_view() {
		front.y = 0;
		pitch = 0;
		view_2d = !view_2d;
	}
	void toggle_physics() {
		physics_enabled = !physics_enabled;
	}
};
extern Camera camera;

void camera_update_rotation(float xoffset, float yoffset);
void camera_update_position(float deltaTime);

#endif
