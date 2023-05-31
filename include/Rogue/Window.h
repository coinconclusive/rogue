#ifndef ROGUE_WINDOW_H_
#define ROGUE_WINDOW_H_
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef uint32_t RoguePixel;

typedef struct {
	size_t width, height;
	RoguePixel *buffer;
	struct RogueWindowImpl *impl_;
} RogueWindow;

typedef enum RogueKey RogueKey;
typedef enum {
	ROGUE_KEY_STATE_NONE = 0,
	ROGUE_KEY_STATE_PRESS = 1,
	ROGUE_KEY_STATE_RELEASE = 2,
	ROGUE_KEY_STATE_REPEAT = 3,
} RogueKeyState;

void RogueWindow_Init(RogueWindow *self, size_t width, size_t height);
void RogueWindow_DeInit(RogueWindow *self);
[[nodiscard]] bool RogueWindow_ShouldStop(RogueWindow *self);
void RogueWindow_Clear(RogueWindow *self, float r, float g, float b, float a);
void RogueWindow_Refresh(RogueWindow *self);
[[nodiscard]] bool RogueWindow_IsKeyDown(RogueWindow *self, RogueKey key);
[[nodiscard]] RogueKeyState RogueWindow_GetKeyState(RogueWindow *self, RogueKey key);
[[nodiscard]] float RogueWindow_GetTime(RogueWindow *self);

enum RogueKey {
	ROGUE_KEY_SPACE = 32,
	ROGUE_KEY_APOSTROPHE = 39, /* ' */
	ROGUE_KEY_COMMA = 44, /* , */
	ROGUE_KEY_MINUS = 45, /* - */
	ROGUE_KEY_PERIOD = 46, /* . */
	ROGUE_KEY_SLASH = 47, /* / */
	ROGUE_KEY_0 = 48,
	ROGUE_KEY_1 = 49,
	ROGUE_KEY_2 = 50,
	ROGUE_KEY_3 = 51,
	ROGUE_KEY_4 = 52,
	ROGUE_KEY_5 = 53,
	ROGUE_KEY_6 = 54,
	ROGUE_KEY_7 = 55,
	ROGUE_KEY_8 = 56,
	ROGUE_KEY_9 = 57,
	ROGUE_KEY_SEMICOLON = 59, /* ; */
	ROGUE_KEY_EQUAL = 61, /* = */
	ROGUE_KEY_A = 65,
	ROGUE_KEY_B = 66,
	ROGUE_KEY_C = 67,
	ROGUE_KEY_D = 68,
	ROGUE_KEY_E = 69,
	ROGUE_KEY_F = 70,
	ROGUE_KEY_G = 71,
	ROGUE_KEY_H = 72,
	ROGUE_KEY_I = 73,
	ROGUE_KEY_J = 74,
	ROGUE_KEY_K = 75,
	ROGUE_KEY_L = 76,
	ROGUE_KEY_M = 77,
	ROGUE_KEY_N = 78,
	ROGUE_KEY_O = 79,
	ROGUE_KEY_P = 80,
	ROGUE_KEY_Q = 81,
	ROGUE_KEY_R = 82,
	ROGUE_KEY_S = 83,
	ROGUE_KEY_T = 84,
	ROGUE_KEY_U = 85,
	ROGUE_KEY_V = 86,
	ROGUE_KEY_W = 87,
	ROGUE_KEY_X = 88,
	ROGUE_KEY_Y = 89,
	ROGUE_KEY_Z = 90,
	ROGUE_KEY_LEFT_BRACKET = 91, /* [ */
	ROGUE_KEY_BACKSLASH = 92, /* \ */
	ROGUE_KEY_RIGHT_BRACKET = 93, /* ] */
	ROGUE_KEY_GRAVE_ACCENT = 96, /* ` */
	ROGUE_KEY_WORLD_1 = 161, /* non-US #1 */
	ROGUE_KEY_WORLD_2 = 162, /* non-US #2 */
	ROGUE_KEY_MAX_ = 256
};

#endif // ROGUE_WINDOW_H_
