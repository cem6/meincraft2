#include "debug.h"

Debug debug;

void debug_init(SDL_Window* window, SDL_GLContext context) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	ImGui::StyleColorsDark();
	ImGui_ImplSDL3_InitForOpenGL(window, context);
	ImGui_ImplOpenGL3_Init("#version 330");
}

void debug_pass_event(SDL_Event &e) {
	ImGui_ImplSDL3_ProcessEvent(&e);
}

void debug_create_window() {
	// imgui
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL3_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin(" ");
	ImGui::Text("%d %d %d", debug.x, debug.y, debug.z);
	ImGui::Text("frame ms: %d", debug.frame_ms);
	ImGui::Text("rendered chunks: %d", debug.rendered_chunks);
	ImGui::Text("generated chunks: %d", debug.generated_chunks);
	ImGui::Text("dirty chunks: %d", debug.dirty_chunks);
	ImGui::End();
}

void debug_render() {
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
