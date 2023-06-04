#ifndef RG_RENDERER_H_
#define RG_RENDERER_H_
#include <Rogue/Window.h>
#include <Rogue/Core.h>

typedef struct {
	RgSize symbolWidth, symbolHeight;
	RgSize *fontAsciiMap;
	RgSize symbolCount;
	const uint8_t *symbolBitmaps;
} RgFont;

typedef struct [[gnu::packed]] {
	char value;
	uint8_t color;
} RgSymbol;

typedef struct {
	RgWindow *window; /* window the renderer renders to. */
	RgSize width, height; /* buffer dimensions in symbols. */
	RgSymbol *buffer; /* symbol buffer. owned by the renderer. */
	RgFont *font; /* font to render with. owned by the user. */
	RgPixel *palette; /* palette that symbol colors refer to. owned by the user. TODO */
	struct { RgInt x, y; } screenOffset; /* offset of the screen. */
	RgPixel borderColor; /* color of the border around the screen. */
	RgBool drawBorder; /* whether to draw a border around the screen. */
} RgRenderer;

void RgRenderer_Init(RgRenderer *self, RgWindow *window, size_t width, size_t height, RgFont *font);
void RgRenderer_Refresh(RgRenderer *self);
void RgRenderer_DeInit(RgRenderer *self);

#endif // RG_RENDERER_H_
