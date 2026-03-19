#include "camera.h"

Camera camera;

void camera_update_rotation(float xoffset, float yoffset) {
	camera.yaw += xoffset * SENSITIVITY;
	camera.pitch -= yoffset * SENSITIVITY;

	// constrain -> cant flip
	if (camera.pitch > 89.0f) camera.pitch = 89.0f;
	if (camera.pitch < -89.0f) camera.pitch = -89.0f;
	// std::cout << "yaw: " << camera.yaw << "\npitch: " << camera.pitch << std::endl;

	glm::vec3 dir;
	dir.x = cos(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
	dir.y = sin(glm::radians(camera.pitch));
	dir.z = sin(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
	camera.front = glm::normalize(dir);
}

void camera_update_position(float deltaTime) {
	float speed = MOVESPEED * deltaTime;
	const bool *state = SDL_GetKeyboardState(NULL);

	if (state[SDL_SCANCODE_W]) camera.pos += speed * camera.front;
	if (state[SDL_SCANCODE_S]) camera.pos -= speed * camera.front;
	if (state[SDL_SCANCODE_A]) camera.pos -= glm::normalize(glm::cross(camera.front, camera.up)) * speed;
	if (state[SDL_SCANCODE_D]) camera.pos += glm::normalize(glm::cross(camera.front, camera.up)) * speed;
	if (state[SDL_SCANCODE_SPACE]) camera.pos.y += speed;
	if (state[SDL_SCANCODE_LCTRL]) camera.pos.y -= speed;
}
