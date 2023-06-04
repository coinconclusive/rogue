#include <Rogue/Core.h>
#include <Rogue/Window.h>
#include <Rogue/Renderer.h>

extern const uint8_t font8x8_basic[128][8];

void ClearBuffer(RgRenderer *renderer, char c, uint8_t col) {
	for (RgSize i = 0; i < renderer->width * renderer->height; ++i)
		renderer->buffer[i] = (RgSymbol){ .value = c, .color = col };
}

void ClearWindowBuffer(RgWindow *window, RgPixel color) {
	for (RgSize i = 0; i < window->width * window->height; ++i)
		window->buffer[i] = color;
}

static bool wasKeyDown[RG_KEY_MAX_] = {0};

typedef struct {
	struct { RgInt x, y; } player;
} World;

static inline void DrawSymbol(RgRenderer *renderer, RgInt x, RgInt y, char c, uint8_t col) {
	if (x < 0 || y < 0 || x >= renderer->width || y >= renderer->height)
		return;
	renderer->buffer[x + y * renderer->width] = (RgSymbol){ .value = c, .color = col };
}

void DrawWorld(World *world, RgRenderer *renderer, float deltaTime) {
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

	if (RgWindow_GetKeyState(renderer->window, RG_KEY_A) == RG_KEY_STATE_PRESS)
		world->player.x -= 1;
	else if (RgWindow_GetKeyState(renderer->window, RG_KEY_D) == RG_KEY_STATE_PRESS)
		world->player.x += 1;

	if (RgWindow_GetKeyState(renderer->window, RG_KEY_W) == RG_KEY_STATE_PRESS)
		world->player.y -= 1;
	else if (RgWindow_GetKeyState(renderer->window, RG_KEY_S) == RG_KEY_STATE_PRESS)
		world->player.y += 1;
}

int main(int argc, char *argv[]) {
	RgWindow window;
	RgWindow_Init(&window, &(RgWindowInitInfo){
		.width = 640,
		.height = 480,
		.scaleX = 2.0f,
		.scaleY = 2.0f,
	});

	RgFont font = {
		.symbolWidth = 10, .symbolHeight = 10,
		.fontAsciiMap = NULL,
		.symbolCount = 128,
		.symbolBitmaps = (const uint8_t*)font8x8_basic
	};

	RgRenderer renderer;
	RgRenderer_Init(&renderer, &window, 16, 16, &font);
	renderer.palette = (uint32_t[]){
		0x000000,
		0xEEEEEE,
		0x2112E2,
		0xE22112,
		0
	};

	World world = {0};
	world.player.y = 2;

	float lastTime = RgWindow_GetTime(&window);

	while (!RgWindow_ShouldStop(&window)) {
		float currentTime = RgWindow_GetTime(&window);
		float deltaTime = currentTime - lastTime;

		ClearBuffer(&renderer, '\0', 0);
		DrawWorld(&world, &renderer, deltaTime);

		renderer.screenOffset.x
			= (window.width / window.scale.x - renderer.font->symbolWidth * renderer.width) / 2;
		renderer.screenOffset.y
			= (window.height / window.scale.y - renderer.font->symbolHeight * renderer.height) / 2;

		RgRenderer_Refresh(&renderer);
		RgWindow_Refresh(&window);
	}

	RgRenderer_DeInit(&renderer);
	RgWindow_DeInit(&window);
}
