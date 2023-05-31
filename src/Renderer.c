#include <Rogue/Renderer.h>
#include <Rogue/Core.h>

void RogueRenderer_Init(RogueRenderer *self, RogueWindow *window, size_t width, size_t height, RogueFont *font) {
	self->width = width;
	self->height = height;
	self->font = font;
	self->window = window;
	self->buffer = RogueAllocArray(sizeof(*self->buffer), self->width * self->height);
}

void RogueRenderer_DeInit(RogueRenderer *self) {
	RogueDeAlloc(self->buffer);
}

void RogueRenderer_Refresh(RogueRenderer *self) {
	for (size_t sy = 0; sy < self->height; ++sy) {
		for (size_t sx = 0; sx < self->width; ++sx) {
			RogueSymbol symbol = self->buffer[sy * self->width + sx];
			uint32_t col = self->palette[symbol.color];

			const uint8_t *bitmap = NULL;
			if (self->font->fontAsciiMap != NULL) { 
				/* bitmap = ... */
			} else {
				bitmap = &self->font->symbolBitmaps[symbol.value * 8];
			}

			size_t offset
				= sx * self->font->symbolWidth +
				  sy * self->font->symbolHeight * self->window->width;

			for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
					self->window->buffer[offset + x + y * self->window->width]
						= (bitmap[y] & (1 << x)) ? col : 0;
        }
    	}
		}
	}
}
