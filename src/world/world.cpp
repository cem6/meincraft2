#include "world.h"

#include "noise/FastNoiseLite.h"
#include "../ui/debug.h"

#include "player.h"

#include <chrono>
std::chrono::steady_clock::time_point startTime;
bool isMeasuring = false;
int max_dc = 0;

World world;
FastNoiseLite noise;

glm::ivec3 neighbors[6] = {
	{1.0, 0.0, 0.0},
	{-1.0, 0.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, -1.0, 0.0},
	{0.0, 0.0, 1.0},
	{0.0, 0.0, -1.0}
};


float _get_noise_height(float x, float z) {
    float total = 0;
    float frequency = 0.6f; // stretched
    float amplitude = 1.5f; // tall
    int octaves = 6; // layers
    float maxValue = 0;      

    for(int i = 0; i < octaves; i++) {
        total += noise.GetNoise(x * frequency, z * frequency) * amplitude;
        
        maxValue += amplitude;
        amplitude *= 0.5f; // layer is half as tall
        frequency *= 2.0f; // layer is twice as detailed
    }

	if (total > 0) total = total*total;
	else total *= 0.8;
	return (total / maxValue + 1) * CHUNK_SIZE_X + 16;
}

// TODO: move to chunk.
Chunk* _generate_blockdata(glm::ivec3 pos) {
	Chunk *c = new Chunk();
	chunk_init(c, pos);

	int xOffset = pos.x * CHUNK_SIZE_X;
	int yOffset = pos.y * CHUNK_SIZE_Y;
	int zOffset = pos.z * CHUNK_SIZE_Z;

	c->solid = true;
	c->dirty = false;

	for (int x = 0; x < CHUNK_SIZE_X; x++) {
		for (int z = 0; z < CHUNK_SIZE_Z; z++) {
			int height = _get_noise_height(xOffset + x, zOffset + z);
			// world.heightMap[glm::ivec2(pos.x, pos.z)] = std::max(height, world.heightMap[glm::ivec2(pos.x, pos.z)]);

			for (int y = 0; y < CHUNK_SIZE_Y; y++) {
				int gy = yOffset + y;

				// ??????????????????????
				if (gy > 1.2*WATER_LEVEL && gy <= height) c->blocks[chunk_pos_to_idx(x, y, z)] = STONE;
				// else if (gy < height - 3) 	c->blocks[chunk_pos_to_idx(x, y, z)] = STONE;
				else if (gy == height && height > WATER_LEVEL + 3) c->blocks[chunk_pos_to_idx(x, y, z)] = GRASS;
				else if (gy <= height && height <= WATER_LEVEL + 3) c->blocks[chunk_pos_to_idx(x, y, z)] = SAND;
				else if (gy < height) c->blocks[chunk_pos_to_idx(x, y, z)] = DIRT;
				else if (gy == WATER_LEVEL)	c->blocks[chunk_pos_to_idx(x, y, z)] = WATER;
				else {
					c->solid = false;
					c->blocks[chunk_pos_to_idx(x, y, z)] = AIR;
				}

			}

		}
	}

	return world.chunkMap[pos] = c;
}

void _generate_initial_world(int s) {
	// create all block data
	for (int cy = 0; cy < s; cy++) {
		for (int cx = -s; cx < s; cx++) {
			for (int cz = -s; cz < s; cz++) {
				Chunk *c = new Chunk();
				chunk_init(c, glm::ivec3(cx, cy, cz));
				_generate_blockdata(glm::ivec3(cx, cy, cz));
			}
		}
	}

	// create meshes (neighbor blocks have to exist)
	for (auto &[_, c] : world.chunkMap) chunk_generate_mesh(c);
}

void world_init() {
	noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2S);
	_generate_initial_world(1);
	player_init();
}

void world_destroy() {
	for (auto &[_, c] : world.chunkMap) {
		glDeleteVertexArrays(1, &c->vao);	
		glDeleteBuffers(1, &c->vbo);
		delete c;
	}
	world.chunkMap.clear();
	world.dirtyChunks.clear();
}

blocktype world_get_block(glm::ivec3 globalPos) {
	// chunk position
    glm::ivec3 cp (
		(globalPos.x >= 0) ? (globalPos.x / CHUNK_SIZE_X) : ((globalPos.x - CHUNK_SIZE_X + 1) / CHUNK_SIZE_X),
		(globalPos.y >= 0) ? (globalPos.y / CHUNK_SIZE_Y) : ((globalPos.y - CHUNK_SIZE_Y + 1) / CHUNK_SIZE_Y),
		(globalPos.z >= 0) ? (globalPos.z / CHUNK_SIZE_Z) : ((globalPos.z - CHUNK_SIZE_Z + 1) / CHUNK_SIZE_Z)
	);

	// block position in chunk
    glm::ivec3 lp (
		((globalPos.x % CHUNK_SIZE_X) + CHUNK_SIZE_X) % CHUNK_SIZE_X,
		((globalPos.y % CHUNK_SIZE_Y) + CHUNK_SIZE_Y) % CHUNK_SIZE_Y,
		((globalPos.z % CHUNK_SIZE_Z) + CHUNK_SIZE_Z) % CHUNK_SIZE_Z
	);

    if (world.chunkMap.count(cp))
        return world.chunkMap[cp]->blocks[chunk_pos_to_idx(lp)];

    return AIR;
}

// TODO: move somwhere else (to chunk ???)
bool world_chunk_visible(glm::ivec3 pos) {
    if (!world.chunkMap[pos]->solid) return true; // not solid -> has air -> not hidden

    for (int i = 0; i < 6; i++) {
        glm::ivec3 neiPos = pos + neighbors[i];
        
		// on border -> visible
        if (world.chunkMap.find(neiPos) == world.chunkMap.end()) return true;
		// has neighbor that isnt solid -> visible
        if (!world.chunkMap[neiPos]->solid) return true;
    }

    return false; // 6 solid neighbors -> hidden
}

void world_update_load_queue(glm::vec3 cameraPos) {
	// generate time test START
	if (!world.dirtyChunks.empty() && !isMeasuring) {
		startTime = std::chrono::steady_clock::now();
		isMeasuring = true;
	}

	int camX = floor(cameraPos.x / CHUNK_SIZE_X);
	int camY = floor(cameraPos.y / CHUNK_SIZE_Y);
	int camZ = floor(cameraPos.z / CHUNK_SIZE_Z);

	for (int d = 0; d <= RENDER_DISTANCE; d++) {
		for (int x = -d; x <= d; x++) {
			for (int z = -d; z <= d; z++) {
				if (std::abs(x) != d && std::abs(z) != d) continue;

				int height = _get_noise_height((camX + x) * CHUNK_SIZE_X, (camZ + z) * CHUNK_SIZE_Z);

				for (int y = RENDER_DISTANCE; y >= -RENDER_DISTANCE; y--) {
					if ((camY + y - 1) * CHUNK_SIZE_Y > height) continue; // dont generate sky chunks
					if ((camY + y) * CHUNK_SIZE_Y < MIN_Y) continue; // dont generate zu weit unten

					glm::ivec3 pos(camX + x, camY + y, camZ + z);
					if (!world.chunkMap.count(pos) && !world.pendingChunks.count(pos)) {
						world.loadQ.push_back(pos);
						world.pendingChunks.insert(pos);
					}
				}
			}
		}
	}
}

void world_process_blockdata_generation(glm::vec3 cameraPos) {
	if (world.loadQ.empty()) {
		debug.dirty_chunks = world.dirtyChunks.size();
		return;
	};

	// TODO: make single sort function
	int limit = std::min((int)world.loadQ.size(), MAX_BLOCKDATA_GENERATIONS_PER_FRAME);
	std::nth_element(world.loadQ.begin(), world.loadQ.begin() + limit, world.loadQ.end(), [&cameraPos](glm::ivec3 &a, glm::ivec3 &b) {
		bool visA = culling_chunk_visible(a);
		bool visB = culling_chunk_visible(b);
		if (visA != visB) return visA; // not both visible -> prioritize visible

		glm::vec3 c = glm::ivec3(floor(cameraPos.x / CHUNK_SIZE_X), 
							floor(cameraPos.y / CHUNK_SIZE_Y), 
							floor(cameraPos.z / CHUNK_SIZE_Z));
		// manhattan
		// int distA = std::abs(a.x - c.x) + std::abs(a.y - c.y) + std::abs(a.z - c.z);
		// int distB = std::abs(b.x - c.x) + std::abs(b.y - c.y) + std::abs(b.z - c.z);

		// euclidean (ohne sqrt)
		// int distA = glm::distance2(glm::vec3(a), c);
		// int distB = glm::distance2(glm::vec3(b), c);

		// return distA < distB;
		
		int distA = std::abs(a.x - c.x) + std::abs(a.z - c.z);
		int distB = std::abs(b.x - c.x) + std::abs(b.z - c.z);
		if (distA != distB) return distA < distB;
		return std::abs(a.y - c.y) < std::abs(b.y - c.y);
	});

	int generated = 0;
	for (glm::ivec3 pos : world.loadQ) {
		if (generated == limit) break;

		world.pendingChunks.erase(pos);
		Chunk *c = _generate_blockdata(pos);

		// mark neighbors dirty for mesh generation
		for (int i = 0; i < 6; i++) {
			glm::ivec3 neiPos = pos + neighbors[i];
			auto it = world.chunkMap.find(neiPos);

			if (it != world.chunkMap.end()) {
				Chunk *nc = it->second;
				// mark neighbors need mesh update
				if (!nc->dirty) {
					nc->dirty = true;
					world.dirtyChunks.push_back(nc);
				}
			}
		}
		generated++;
	}
	world.loadQ.erase(world.loadQ.begin(), world.loadQ.begin() + generated);

	debug.dirty_chunks = world.dirtyChunks.size();
	max_dc = std::max(max_dc, (int)world.dirtyChunks.size());
}

void world_process_mesh_generation(glm::vec3 cameraPos) {
	if (world.dirtyChunks.empty()) return;

	int limit = std::min((int)world.dirtyChunks.size(), MAX_MESH_GENERATIONS_PER_FRAME);
	std::nth_element(world.dirtyChunks.begin(), world.dirtyChunks.begin() + limit, world.dirtyChunks.end(), [&cameraPos](Chunk *a, Chunk *b) {
		bool visA = culling_chunk_visible(a->pos);
		bool visB = culling_chunk_visible(b->pos);
		if (visA != visB) return visA; // not both visible -> prioritize visible

		glm::vec3 c = glm::ivec3(floor(cameraPos.x / CHUNK_SIZE_X), 
							floor(cameraPos.y / CHUNK_SIZE_Y), 
							floor(cameraPos.z / CHUNK_SIZE_Z));
		// int distA = std::abs(a->pos.x - c.x) + std::abs(a->pos.y - c.y) + std::abs(a->pos.z - c.z);
		// int distB = std::abs(b->pos.x - c.x) + std::abs(b->pos.y - c.y) + std::abs(b->pos.z - c.z);

		// int distA = glm::distance2(glm::vec3(a->pos), c);
		// int distB = glm::distance2(glm::vec3(b->pos), c);

		int distA = std::abs(a->pos.x - c.x) + std::abs(a->pos.z - c.z);
		int distB = std::abs(b->pos.x - c.x) + std::abs(b->pos.z - c.z);
		if (distA != distB) return distA < distB;
		return std::abs(a->pos.y - c.y) < std::abs(b->pos.y - c.y);
	});


	int generated = 0;
	for (Chunk *c : world.dirtyChunks) {
		if (generated == limit) break;
		chunk_generate_mesh(c);
		c->dirty = false;
		generated++;
	}
	world.dirtyChunks.erase(world.dirtyChunks.begin(), world.dirtyChunks.begin() + generated);

	debug.generated_chunks = world.chunkMap.size();

	// generate time test END
	if (isMeasuring && world.dirtyChunks.empty()) {
		auto endTime = std::chrono::steady_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

		std::cout << "ms: " << duration.count() << ", max dirty chunks: " << max_dc << std::endl;
		isMeasuring = false;
	}
}

void world_add_block(glm::ivec3 pos, blocktype b) {

	glm::ivec3 chunkPos = {
		(int)floor((float(pos.x) / CHUNK_SIZE_X)),
		(int)floor((float(pos.y) / CHUNK_SIZE_Y)),
		(int)floor((float(pos.z) / CHUNK_SIZE_Z))
	};

	if (!world.chunkMap.count(chunkPos)) return;
	Chunk *c = world.chunkMap[chunkPos];

	// global -> local pos in chunk
	int lx = ((pos.x % CHUNK_SIZE_X) + CHUNK_SIZE_X) % CHUNK_SIZE_X;
	int ly = ((pos.y % CHUNK_SIZE_Y) + CHUNK_SIZE_Y) % CHUNK_SIZE_Y;
	int lz = ((pos.z % CHUNK_SIZE_Z) + CHUNK_SIZE_Z) % CHUNK_SIZE_Z;

	c->blocks[chunk_pos_to_idx(lx, ly, lz)] = b;
	if (!c->dirty) {
		c->dirty = true;
		world.dirtyChunks.push_back(c);		
	}

	// mark neighbors dirty
	auto mark = [&](int dx, int dy, int dz) {
		glm::ivec3 pos = chunkPos + glm::ivec3(dx, dy, dz);
		auto it = world.chunkMap.find(pos);
		if (it != world.chunkMap.end()) {
			Chunk *nc = it->second;
			if (!nc->dirty) {
				nc->dirty = true;
				world.dirtyChunks.push_back(nc);
			}
		}
	};
	if (lx == 0) 				mark(-1, 0, 0);
	if (lx == CHUNK_SIZE_X - 1) mark(1, 0, 0);
	if (ly == 0) 				mark(0, -1, 0);
	if (ly == CHUNK_SIZE_Y - 1) mark(0, 1, 0);
	if (lz == 0) 				mark(0, 0, -1);
	if (lz == CHUNK_SIZE_Z - 1) mark(0, 0, 1);

	// world_update_load_queue(pos);
}

void world_remove_block(glm::ivec3 pos) {
	world_add_block(pos, AIR);
	world_update_load_queue(pos);
}

Raycast world_get_raycast(glm::vec3 camPos, glm::vec3 camDir, float maxDist) {
	Raycast raycast;

	// current block
	// ! da vertex coords -0.5 bis 0.5 muss auch origin
	// glm::vec3 origin = camera.pos + (camera.front * 0.1f) + 0.5f;
	// glm::ivec3 pos = glm::floor(origin);
	// glm::vec3 dir = camera.front;
	glm::vec3 origin = camPos + (camDir * 0.1f) + 0.5f;
	glm::ivec3 pos = glm::floor(origin);
	glm::vec3 dir = camDir;

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
