#ifndef PLAYER_H
#define PLAYER_H

#include "../util/util.h"
#include "../render/camera.h"
#include "world.h"


struct Player {
	int selectedBlockIdx = 0;
};

void player_init();
void player_next_block();
void player_prev_block();
void player_mark_looking_at(GLint aimPosLoc, GLint aimNormLoc);
void player_try_place_block();
void player_try_break_block();

#endif
