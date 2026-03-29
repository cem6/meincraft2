#ifndef UTIL_H
#define UTIL_H

#include <glad/glad.h>
#include <SDL3/SDL.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp>

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <limits>

#include <unordered_set>
#include <vector>
#include <deque>

#include "defines.h"


struct Vec3Hasher {
	std::size_t operator()(const glm::ivec3 &v) const {
		std::size_t h = 0;
		auto hash_int = std::hash<int>{};

		h ^= hash_int(v.x) + 0x9e3779b9 + (h << 6) + (h >> 2);
		h ^= hash_int(v.y) + 0x9e3779b9 + (h << 6) + (h >> 2);
		h ^= hash_int(v.z) + 0x9e3779b9 + (h << 6) + (h >> 2);

		return h;
	}
};

struct Vec2Hasher {
	std::size_t operator()(const glm::ivec2 &v) const {
		std::size_t h = 0;
		auto hash_int = std::hash<int>{};

		h ^= hash_int(v.x) + 0x9e3779b9 + (h << 6) + (h >> 2);
		h ^= hash_int(v.y) + 0x9e3779b9 + (h << 6) + (h >> 2);

		return h;
	}
};



#endif
