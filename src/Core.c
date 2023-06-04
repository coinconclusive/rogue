#include <Rogue/Core.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

void RgFail(const char *fmt, ...) {
	va_list va;
	va_start(va, fmt);
	fputs("\033[1;31m", stderr);
	vfprintf(stderr, fmt, va);
	va_end(va);
	fputs("\033[m\n", stderr);
	exit(1);
}

void RgLogError(const char *fmt, ...) {
	va_list va;
	va_start(va, fmt);
	fputs("\033[1;31m", stderr);
	vfprintf(stderr, fmt, va);
	va_end(va);
	fputs("\033[m\n", stderr);
}

void *RgAllocArray(size_t elemSize, size_t numElems) { return calloc(numElems, elemSize); }
void *RgAlloc(size_t size) { return malloc(size); }
void RgDeAlloc(void *ptr) { free(ptr); }
