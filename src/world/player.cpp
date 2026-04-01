#include "player.h"
#include "../ui/debug.h"

Player player;

void player_init() {
	debug.selected_block = blocktype_to_str[placableBlocks[player.selectedBlockIdx]];
}

void player_next_block() {
	player.selectedBlockIdx = (player.selectedBlockIdx + 1) % placableBlocks.size();
	debug.selected_block = blocktype_to_str[placableBlocks[player.selectedBlockIdx]];
}

void player_prev_block() {
	player.selectedBlockIdx = (player.selectedBlockIdx - 1 + placableBlocks.size()) % placableBlocks.size();
	debug.selected_block = blocktype_to_str[placableBlocks[player.selectedBlockIdx]];
}

void player_try_place_block() {
	Raycast r = world_get_raycast(camera.pos, camera.front, RAYCAST_DIST);
	if (r.hit) world_add_block(r.blockPos + glm::ivec3(r.normal), placableBlocks[player.selectedBlockIdx]);
}

void player_try_break_block() {
	Raycast r = world_get_raycast(camera.pos, camera.front, RAYCAST_DIST);
	if (r.hit) world_remove_block(r.blockPos);
}

void player_mark_looking_at(GLint aimPosLoc, GLint aimNormLoc) {
	Raycast r = world_get_raycast(camera.pos, camera.front, RAYCAST_DIST);
	if (r.hit){
		glUniform3i(aimPosLoc, r.blockPos.x, r.blockPos.y, r.blockPos.z);
		glUniform3i(aimNormLoc, r.normal.x, r.normal.y, r.normal.z);
	} 
	else glUniform3i(aimPosLoc, 0, INT_MIN, 0);
}


