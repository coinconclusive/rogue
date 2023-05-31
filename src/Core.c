#include <Rogue/Core.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

void RogueFail(const char *fmt, ...) {
	va_list va;
	va_start(va, fmt);
	fputs("\033[1;31m", stderr);
	vfprintf(stderr, fmt, va);
	va_end(va);
	fputs("\033[m\n", stderr);
	exit(1);
}

void RogueLogError(const char *fmt, ...) {
	va_list va;
	va_start(va, fmt);
	fputs("\033[1;31m", stderr);
	vfprintf(stderr, fmt, va);
	va_end(va);
	fputs("\033[m\n", stderr);
}

void *RogueAllocArray(size_t elemSize, size_t numElems) { return calloc(numElems, elemSize); }
void *RogueAlloc(size_t size) { return malloc(size); }
void RogueDeAlloc(void *ptr) { free(ptr); }
