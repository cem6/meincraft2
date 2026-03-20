// window
#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800

#define TARGET_FPS 60
constexpr int FRAME_DELAY = (1000 / TARGET_FPS);

// renderer
#define RENDER_DISTANCE 8 // radius

// camera
constexpr float MOVESPEED = 20.0f;
constexpr float SENSITIVITY = 0.1f;

// chunk
#define CHUNK_SIZE_X 32
#define CHUNK_SIZE_Y 32
#define CHUNK_SIZE_Z 32
constexpr int CHUNK_VOLUME (CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z);

// world
#define MAX_GENERATIONS_PER_FRAME 4

