// window
#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800

#define TARGET_FPS 30
constexpr int FRAME_DELAY = (1000 / TARGET_FPS);

// renderer
#define RENDER_DISTANCE 16 // radius
#define FRUSTUM_CULLING true

// camera
constexpr float MOVESPEED = 15.0f;
constexpr float SENSITIVITY = 0.1f;
#define RAYCAST_DIST 20.0f

// chunk
#define CHUNK_SIZE_X 32
#define CHUNK_SIZE_Y 32
#define CHUNK_SIZE_Z 32
constexpr int CHUNK_VOLUME (CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z);

// world
#define MAX_BLOCKDATA_GENERATIONS_PER_FRAME 5
#define MAX_MESH_GENERATIONS_PER_FRAME (MAX_BLOCKDATA_GENERATIONS_PER_FRAME + 1)
// #define MIN_Y (-CHUNK_SIZE_Y * 1)
#define MIN_Y 0
#define WATER_LEVEL 44
