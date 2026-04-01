#include "camera.h"

#include "../ui/debug.h"

Camera camera;

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

bool _check_collision(glm::vec3 pos) {
	float w = 0.4f, h = 1.2f;
	float eh = h - 0.1f;

	for (float x = -w; x <= w; x += 2*w) {
		for (float z = -w; z <= w; z += 2*w) {
			for (float y = h-eh; y <= h; y += eh/2) {
				glm::ivec3 blockPos = glm::floor(pos + glm::vec3(x, y - eh, z));
				blocktype b = world_get_block(blockPos);
				if (b != AIR && b != WATER) return true;
			}
		}
	}

	return false;
}

void camera_move_physics(float deltaTime) {
	float speed = MOVESPEED/2 * deltaTime;
	const bool *state = SDL_GetKeyboardState(NULL);
	if (state[SDL_SCANCODE_LSHIFT]) speed *= 2;

	glm::vec3 dir(0.0f);

	glm::vec3 forward = glm::normalize(glm::vec3(camera.front.x, 0.0f, camera.front.z));
	glm::vec3 right = glm::normalize(glm::cross(forward, camera.up));

	if (state[SDL_SCANCODE_W]) 	dir += forward;
	if (state[SDL_SCANCODE_S]) 	dir -= forward;
	if (state[SDL_SCANCODE_A]) 	dir -= right;
	if (state[SDL_SCANCODE_D]) 	dir += right;
	// if (state[SDL_SCANCODE_W]) 	dir += camera.front;
	// if (state[SDL_SCANCODE_S]) 	dir -= camera.front;
	// if (state[SDL_SCANCODE_A]) 	dir -= glm::normalize(glm::cross(camera.front, camera.up));
	// if (state[SDL_SCANCODE_D]) 	dir += glm::normalize(glm::cross(camera.front, camera.up));

	// prevent faster diagonal movement
	if (glm::length(dir) > 0.0f) dir = glm::normalize(dir) * speed;

	glm::vec3 np = camera.pos;
	np.x += dir.x;
	if (!_check_collision(np)) camera.pos.x = np.x;

	np = camera.pos;
	np.z += dir.z;
	if (!_check_collision(np)) camera.pos.z = np.z;

	if (state[SDL_SCANCODE_SPACE]) dir.y += speed;
	else dir.y -= speed * 2;

	np = camera.pos;
	np.y += dir.y;
	if (!_check_collision(np)) camera.pos.y = np.y;

	debug.x = camera.pos.x;
	debug.y = camera.pos.y;
	debug.z = camera.pos.z;
}

void camera_move_fly(float deltaTime) {
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

void camera_update_position(float deltaTime) {
	if (camera.physics_enabled) camera_move_physics(deltaTime);
	else camera_move_fly(deltaTime);
}

