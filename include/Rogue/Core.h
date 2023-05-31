#ifndef ROGUE_CORE_H_
#define ROGUE_CORE_H_
#include <stddef.h>
#include <stdint.h>

[[gnu::format(printf, 1, 2)]] [[noreturn]] void RogueFail(const char *fmt, ...);
[[gnu::format(printf, 1, 2)]] void RogueLogError(const char *fmt, ...);
[[gnu::malloc]] void *RogueAlloc(size_t size);
[[gnu::alloc_size(1, 2)]] void *RogueAllocArray(size_t elemSize, size_t numElems);
void RogueDeAlloc(void *ptr);

static inline void RogueMemFill(uint8_t byte, void *start, size_t size) {
	__builtin_memset(start, byte, size);
}

#endif // ROGUE_CORE_H_
