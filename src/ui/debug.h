#ifndef DEBUG_H
#define DEBUG_H

#include "../util/util.h"

#include "text.h"

extern struct Debug {
	int x, y, z;
	int frame_ms;
	int rendered_chunks;
	int generated_chunks;
	int dirty_chunks;

	bool visible = true;
	void toggle_visible() {
		visible = !visible;
	}
} debug;

void debug_draw();

#endif
