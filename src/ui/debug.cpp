#include "debug.h"

Debug debug;

void debug_draw() {
	if (!debug.visible) return;
	char buffer[128];
	float step = (48 * 0.3f + 20);

	snprintf(buffer, sizeof(buffer), "%d %d %d", debug.x, debug.y, debug.z);
	text_draw_box(buffer, 15.0f, WINDOW_HEIGHT - 30.0f - 0*step, 0.3f, glm::vec4(1, 1, 1, 1), glm::vec4(0, 0, 0, .8));

	snprintf(buffer, sizeof(buffer), "frame ms: %d", debug.frame_ms);
	text_draw_box(buffer, 15.0f, WINDOW_HEIGHT - 30.0f - 1*step, 0.3f, glm::vec4(1, 1, 1, 1), glm::vec4(0, 0, 0, .8));

	snprintf(buffer, sizeof(buffer), "rendered chunks: %d", debug.rendered_chunks);
	text_draw_box(buffer, 15.0f, WINDOW_HEIGHT - 30.0f - 2*step, 0.3f, glm::vec4(1, 1, 1, 1), glm::vec4(0, 0, 0, .8));

	snprintf(buffer, sizeof(buffer), "generated chunks: %d", debug.generated_chunks);
	text_draw_box(buffer, 15.0f, WINDOW_HEIGHT - 30.0f - 3*step, 0.3f, glm::vec4(1, 1, 1, 1), glm::vec4(0, 0, 0, .8));

	snprintf(buffer, sizeof(buffer), "dirty chunks: %d", debug.dirty_chunks);
	text_draw_box(buffer, 15.0f, WINDOW_HEIGHT - 30.0f - 4*step, 0.3f, glm::vec4(1, 1, 1, 1), glm::vec4(0, 0, 0, .8));
}
