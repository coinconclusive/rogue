#include <Rogue/Renderer.h>
#include <Rogue/Core.h>

void RgRenderer_Init(RgRenderer *self, RgWindow *window, RgSize width, RgSize height, RgFont *font) {
	self->width = width;
	self->height = height;
	self->font = font;
	self->window = window;
	self->drawBorder = true;
	self->borderColor = 0xFFFFFF;
	self->screenOffset.x = 32;
	self->screenOffset.y = 32;
	self->buffer = RgAllocArray(sizeof(*self->buffer), self->width * self->height);
}

void RgRenderer_DeInit(RgRenderer *self) {
	RgDeAlloc(self->buffer);
}

void RgRenderer_Refresh(RgRenderer *self) {
	for (RgInt sy = 0; sy < self->height; ++sy) {
		for (RgInt sx = 0; sx < self->width; ++sx) {
			RgSymbol symbol = self->buffer[sy * self->width + sx];
			uint32_t col = self->palette[symbol.color];

			const uint8_t *bitmap = NULL;
			if (self->font->fontAsciiMap != NULL) { 
				/* bitmap = ... */
			} else {
				bitmap = &self->font->symbolBitmaps[symbol.value * 8];
			}

			RgInt offsetX = sx * (RgInt)self->font->symbolWidth
				+ self->screenOffset.x + (RgInt)(self->font->symbolWidth - 8) / 2;

			RgInt offsetY = sy * (RgInt)self->font->symbolHeight
				+ self->screenOffset.y + (RgInt)(self->font->symbolHeight - 8) / 2;

			if (offsetX >= self->window->bufferWidth || offsetY >= self->window->bufferHeight || offsetX < 0 || offsetY < 0)
				continue;

			RgSize offset = offsetX + offsetY * self->window->bufferWidth;

			for (RgSize y = 0; y < 8; y++) {
				if (offsetY + y >= self->window->bufferHeight) break;
        for (RgSize x = 0; x < 8; x++) {
					if (offsetX + x >= self->window->bufferWidth) break;
					self->window->buffer[offset + x + y * self->window->bufferWidth]
						= (bitmap[y] & (1 << x)) ? col : 0;
        }
    	}
		}
	}

	if (self->drawBorder) {
		RgSize screenWidth = self->width * self->font->symbolWidth;
		RgSize screenHeight = self->width * self->font->symbolHeight;
		RgInt offset;
		
		// draw the upper horizontal line:
		offset = (self->screenOffset.y - 1) * (RgInt)self->window->bufferWidth + (self->screenOffset.x - 1);
		if (offset >= 0 && offset <= self->window->bufferWidth * self->window->bufferHeight) {
			for (RgInt pos = offset; pos < offset + Rg_Min(screenWidth + 2, self->window->bufferWidth); ++pos) {
				self->window->buffer[pos] = self->borderColor;
			}
		}

		offset = Rg_Max(self->screenOffset.y, 0) * (RgInt)self->window->bufferWidth + (self->screenOffset.x - 1);

		// draw the vertical lines:
		for (
			RgSize pos = offset;
			pos < offset + self->window->bufferWidth * Rg_Min(screenHeight - 1, self->window->bufferHeight);
			pos += self->window->bufferWidth
		) {
			self->window->buffer[pos] = self->borderColor;
			self->window->buffer[pos + screenWidth + 1] = self->borderColor;
		}

		// draw the bottom horizontal line:
		offset = (self->screenOffset.y + screenHeight - 1) * self->window->bufferWidth + (self->screenOffset.x - 1);
		if (offset >= 0 && offset <= self->window->bufferWidth * self->window->bufferHeight - screenWidth - 2) {
			for (RgSize pos = offset; pos < offset + screenWidth + 2; ++pos)
				self->window->buffer[pos] = self->borderColor;
		}
	}
}
