#include <Rogue/Core.h>
#include <Rogue/Window.h>
#include <Rogue/Renderer.h>

extern const uint8_t font8x8_basic[128][8];

void ClearBuffer(RogueRenderer *renderer, char c, uint8_t col) {
	for (size_t i = 0; i < renderer->width * renderer->height; ++i)
		renderer->buffer[i] = (RogueSymbol){ .value = c, .color = col };
}

void ClearWindowBuffer(RogueWindow *window, RoguePixel color) {
	for (size_t i = 0; i < window->width * window->height; ++i)
		window->buffer[i] = color;
}

static bool wasKeyDown[ROGUE_KEY_MAX_] = {0};

typedef struct {
	struct { int x, y; } player;
} World;

static inline void DrawSymbol(RogueRenderer *renderer, int x, int y, char c, uint8_t col) {
	if (x < 0 || y < 0 || x >= renderer->width || y >= renderer->height)
		return;
	renderer->buffer[x + y * renderer->width] = (RogueSymbol){ .value = c, .color = col };
}

void DrawWorld(World *world, RogueRenderer *renderer, float deltaTime) {
	DrawSymbol(renderer, world->player.x, world->player.y, '@', 1);
	DrawSymbol(renderer, 10, 10, '~', 3);
	DrawSymbol(renderer, 11, 10, '~', 3);
	DrawSymbol(renderer, 10, 11, '~', 3);
	DrawSymbol(renderer, 11, 11, '~', 3);
	DrawSymbol(renderer, 10,  9, '~', 3);
	DrawSymbol(renderer, 11,  9, '~', 3);
	DrawSymbol(renderer,  9, 10, '~', 3);
	DrawSymbol(renderer, 12,  9, '~', 3);
	DrawSymbol(renderer, 11,  8, '~', 3);
	DrawSymbol(renderer, 5, 8, '^', 2);

	if (RogueWindow_GetKeyState(renderer->window, ROGUE_KEY_A) == ROGUE_KEY_STATE_PRESS)
		world->player.x -= 1;
	else if (RogueWindow_GetKeyState(renderer->window, ROGUE_KEY_D) == ROGUE_KEY_STATE_PRESS)
		world->player.x += 1;

	if (RogueWindow_GetKeyState(renderer->window, ROGUE_KEY_W) == ROGUE_KEY_STATE_PRESS)
		world->player.y -= 1;
	else if (RogueWindow_GetKeyState(renderer->window, ROGUE_KEY_S) == ROGUE_KEY_STATE_PRESS)
		world->player.y += 1;
}

int main(int argc, char *argv[]) {
	RogueWindow window;
	RogueWindow_Init(&window, 640, 480);
	window.scale.x = 2.0f;
	window.scale.y = 2.0f;

	RogueFont font = {
		.symbolWidth = 10, .symbolHeight = 10,
		.fontAsciiMap = NULL,
		.symbolCount = 128,
		.symbolBitmaps = (const uint8_t*)font8x8_basic
	};

	RogueRenderer renderer;
	RogueRenderer_Init(&renderer, &window, 16, 16, &font);
	renderer.palette = (uint32_t[]){
		0x000000,
		0xEEEEEE,
		0x2112E2,
		0xE22112,
		0
	};

	World world = {0};
	world.player.y = 2;

	float lastTime = RogueWindow_GetTime(&window);

	while (!RogueWindow_ShouldStop(&window)) {
		float currentTime = RogueWindow_GetTime(&window);
		float deltaTime = currentTime - lastTime;

		ClearBuffer(&renderer, '\0', 0);

		DrawWorld(&world, &renderer, deltaTime);

		RogueRenderer_Refresh(&renderer);

		RogueWindow_Refresh(&window);
	}

	RogueRenderer_DeInit(&renderer);
	RogueWindow_DeInit(&window);
}
