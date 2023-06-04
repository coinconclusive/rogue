#ifndef RG_CORE_H_
#define RG_CORE_H_
#include <stddef.h>
#include <stdint.h>

[[gnu::format(printf, 1, 2)]] [[noreturn]] void RgFail(const char *fmt, ...);
[[gnu::format(printf, 1, 2)]] void RgLogError(const char *fmt, ...);
[[gnu::malloc]] void *RgAlloc(size_t size);
[[gnu::alloc_size(1, 2)]] void *RgAllocArray(size_t elemSize, size_t numElems);
void RgDeAlloc(void *ptr);

static inline void RgMemFill(uint8_t byte, void *start, size_t size) {
	__builtin_memset(start, byte, size);
}

typedef int_fast32_t RgInt;
typedef size_t RgSize;
typedef _Bool RgBool;

#define RG_FALSE 0
#define RG_TRUE 1

static inline int RgInt_Min(RgInt a, RgInt b) { return a < b ? a : b; }
static inline int RgInt_Max(RgInt a, RgInt b) { return a < b ? b : a; }
static inline RgSize RgSize_Min(RgSize a, RgSize b) { return a < b ? a : b; }
static inline RgSize RgSize_Max(RgSize a, RgSize b) { return a < b ? b : a; }

#define Rg_Min(A, B) _Generic((A), RgInt: RgInt_Min, RgSize: RgSize_Min)((A), (B))
#define Rg_Max(A, B) _Generic((A), RgInt: RgInt_Max, RgSize: RgSize_Max)((A), (B))


#endif // RG_CORE_H_
