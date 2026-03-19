// window
#define WINDOW_WIDTH 900
#define WINDOW_HEIGHT 600

#define TARGET_FPS 60
constexpr int FRAME_DELAY = (1000 / TARGET_FPS);

// camera
constexpr float MOVESPEED = 0.02f;
constexpr float SENSITIVITY = 0.1f;

// chunk
#define CHUNK_SIZE_X 32
#define CHUNK_SIZE_Y 32
#define CHUNK_SIZE_Z 32
constexpr int CHUNK_VOLUME (CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z);
