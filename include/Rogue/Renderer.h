#ifndef ROGUE_RENDERER_H_
#define ROGUE_RENDERER_H_
#include <Rogue/Window.h>

typedef struct {
	size_t symbolWidth, symbolHeight;
	size_t *fontAsciiMap;
	size_t symbolCount;
	const uint8_t *symbolBitmaps;
} RogueFont;

typedef struct [[gnu::packed]] {
	char value;
	uint8_t color;
} RogueSymbol;

typedef struct {
	RogueWindow *window;
	size_t width, height;
	RogueSymbol *buffer;
	RogueFont *font;
	RoguePixel *palette;
} RogueRenderer;

void RogueRenderer_Init(RogueRenderer *self, RogueWindow *window, size_t width, size_t height, RogueFont *font);
void RogueRenderer_Refresh(RogueRenderer *self);
void RogueRenderer_DeInit(RogueRenderer *self);

#endif // ROGUE_RENDERER_H_
