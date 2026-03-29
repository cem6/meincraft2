#include "camera.h"

#include "../util/debug.h"

Camera camera;

// Raycast camera_get_raycast(glm::vec3 origin, glm::vec3 dir, float maxDist) {
Raycast camera_get_raycast(float maxDist) {
	Raycast raycast;

	// current block
	// ! da vertex coords -0.5 bis 0.5 muss auch origin
	glm::vec3 origin = camera.pos + (camera.front * 0.1f) + 0.5f;
	glm::ivec3 pos = glm::floor(origin);
	glm::vec3 dir = camera.front;

	// direction of each axis
	glm::ivec3 step;
	step.x = (dir.x > 0) ? 1 : -1;
	step.y = (dir.y > 0) ? 1 : -1;
	step.z = (dir.z > 0) ? 1 : -1;

	// dist along ray to first solid block
	glm::vec3 tMax;
    tMax.x = (floor(origin.x + (step.x > 0 ? 1 : 0)) - origin.x) / dir.x;
    tMax.y = (floor(origin.y + (step.y > 0 ? 1 : 0)) - origin.y) / dir.y;
    tMax.z = (floor(origin.z + (step.z > 0 ? 1 : 0)) - origin.z) / dir.z;

	// dist along ray to next (1) block
	glm::vec3 tDelta = glm::abs(1.0f / dir);

	float dist = 0;
	while (dist < maxDist) {
		// check hit solid block
		blocktype b = world_get_block(pos);
		if (b != AIR && b != WATER) {
			raycast.hit = true;
			raycast.blockPos = pos;
			return raycast;
		}

        // check closest block in each direction -> go there
		// ! so insgesamt weniger vergleiche
		if (tMax.x < tMax.y) {
			if (tMax.x < tMax.z) {
                pos.x += step.x;
                dist = tMax.x;
                tMax.x += tDelta.x;
                raycast.normal = glm::vec3(-step.x, 0, 0);
            } 
			else {
                pos.z += step.z;
                dist = tMax.z;
                tMax.z += tDelta.z;
                raycast.normal = glm::vec3(0, 0, -step.z);
            }
        } 
		else {
			if (tMax.y < tMax.z) {
                pos.y += step.y;
                dist = tMax.y;
                tMax.y += tDelta.y;
                raycast.normal = glm::vec3(0, -step.y, 0);
            } 
			else {
                pos.z += step.z;
                dist = tMax.z;
                tMax.z += tDelta.z;
                raycast.normal = glm::vec3(0, 0, -step.z);
            }
        }
    }

	// std::cout << raycast.normal.x << ' ' << raycast.normal.y << ' ' << raycast.normal.z << std::endl;
    return raycast;
}

void camera_update_rotation(float xoffset, float yoffset) {
	if (camera.view_2d) return;

	camera.yaw += xoffset * SENSITIVITY;
	camera.pitch -= yoffset * SENSITIVITY;

	// cant flip
	if (camera.pitch > 89.0f) camera.pitch = 89.0f;
	if (camera.pitch < -89.0f) camera.pitch = -89.0f;

	glm::vec3 dir;
	dir.x = cos(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
	dir.y = sin(glm::radians(camera.pitch));
	dir.z = sin(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
	camera.front = glm::normalize(dir);
}

void camera_update_position(float deltaTime) {
	float speed = MOVESPEED * deltaTime;
	const bool *state = SDL_GetKeyboardState(NULL);

	if (state[SDL_SCANCODE_LSHIFT]) speed *= 4;

	// if (state[SDL_SCANCODE_W] && !_check_collision(camera.pos + speed * camera.front)) camera.pos += speed * camera.front;
	if (state[SDL_SCANCODE_W]) camera.pos += speed * camera.front;
	if (state[SDL_SCANCODE_S]) camera.pos -= speed * camera.front;
	if (state[SDL_SCANCODE_A]) camera.pos -= glm::normalize(glm::cross(camera.front, camera.up)) * speed;
	if (state[SDL_SCANCODE_D]) camera.pos += glm::normalize(glm::cross(camera.front, camera.up)) * speed;
	if (state[SDL_SCANCODE_SPACE]) camera.pos.y += speed;
	if (state[SDL_SCANCODE_LCTRL]) camera.pos.y -= speed;

	debug.x = camera.pos.x;
	debug.y = camera.pos.y;
	debug.z = camera.pos.z;
}

