#ifndef RG_WINDOW_H_
#define RG_WINDOW_H_
#include <Rogue/Core.h>

typedef uint32_t RgPixel;

typedef struct {
	RgSize width, height;
	float scaleX, scaleY;
} RgWindowInitInfo;

typedef struct {
	RgSize width, height;
	RgSize bufferWidth, bufferHeight;
	RgPixel *buffer;
	struct { float x, y; } scale;
	struct RgWindowImpl *impl_;
} RgWindow;

typedef enum RgKey RgKey;
typedef enum : uint8_t {
	RG_KEY_STATE_NONE = 0,
	RG_KEY_STATE_PRESS = 1,
	RG_KEY_STATE_RELEASE = 2,
	RG_KEY_STATE_REPEAT = 3,
} RgKeyState;

void RgWindow_Init(RgWindow *self, const RgWindowInitInfo *info);
void RgWindow_DeInit(RgWindow *self);
[[nodiscard]] bool RgWindow_ShouldStop(RgWindow *self);
void RgWindow_Clear(RgWindow *self, float r, float g, float b, float a);
void RgWindow_Refresh(RgWindow *self);
[[nodiscard]] bool RgWindow_IsKeyDown(RgWindow *self, RgKey key);
[[nodiscard]] RgKeyState RgWindow_GetKeyState(RgWindow *self, RgKey key);
[[nodiscard]] float RgWindow_GetTime(RgWindow *self);

enum RgKey {
	RG_KEY_SPACE = 32,
	RG_KEY_APOSTROPHE = 39, /* ' */
	RG_KEY_COMMA = 44, /* , */
	RG_KEY_MINUS = 45, /* - */
	RG_KEY_PERIOD = 46, /* . */
	RG_KEY_SLASH = 47, /* / */
	RG_KEY_0 = 48,
	RG_KEY_1 = 49,
	RG_KEY_2 = 50,
	RG_KEY_3 = 51,
	RG_KEY_4 = 52,
	RG_KEY_5 = 53,
	RG_KEY_6 = 54,
	RG_KEY_7 = 55,
	RG_KEY_8 = 56,
	RG_KEY_9 = 57,
	RG_KEY_SEMICOLON = 59, /* ; */
	RG_KEY_EQUAL = 61, /* = */
	RG_KEY_A = 65,
	RG_KEY_B = 66,
	RG_KEY_C = 67,
	RG_KEY_D = 68,
	RG_KEY_E = 69,
	RG_KEY_F = 70,
	RG_KEY_G = 71,
	RG_KEY_H = 72,
	RG_KEY_I = 73,
	RG_KEY_J = 74,
	RG_KEY_K = 75,
	RG_KEY_L = 76,
	RG_KEY_M = 77,
	RG_KEY_N = 78,
	RG_KEY_O = 79,
	RG_KEY_P = 80,
	RG_KEY_Q = 81,
	RG_KEY_R = 82,
	RG_KEY_S = 83,
	RG_KEY_T = 84,
	RG_KEY_U = 85,
	RG_KEY_V = 86,
	RG_KEY_W = 87,
	RG_KEY_X = 88,
	RG_KEY_Y = 89,
	RG_KEY_Z = 90,
	RG_KEY_LEFT_BRACKET = 91, /* [ */
	RG_KEY_BACKSLASH = 92, /* \ */
	RG_KEY_RIGHT_BRACKET = 93, /* ] */
	RG_KEY_GRAVE_ACCENT = 96, /* ` */
	RG_KEY_WORLD_1 = 161, /* non-US #1 */
	RG_KEY_WORLD_2 = 162, /* non-US #2 */
	RG_KEY_ESCAPE = 256,
	RG_KEY_ENTER = 257,
	RG_KEY_TAB = 258,
	RG_KEY_BACKSPACE = 259,
	RG_KEY_INSERT = 260,
	RG_KEY_DELETE = 261,
	RG_KEY_RIGHT = 262,
	RG_KEY_LEFT = 263,
	RG_KEY_DOWN = 264,
	RG_KEY_UP = 265,
	RG_KEY_PAGE_UP = 266,
	RG_KEY_PAGE_DOWN = 267,
	RG_KEY_HOME = 268,
	RG_KEY_END = 269,
	RG_KEY_CAPS_LOCK = 280,
	RG_KEY_SCROLL_LOCK = 281,
	RG_KEY_NUM_LOCK = 282,
	RG_KEY_PRINT_SCREEN = 283,
	RG_KEY_PAUSE = 284,
	RG_KEY_F1 = 290,
	RG_KEY_F2 = 291,
	RG_KEY_F3 = 292,
	RG_KEY_F4 = 293,
	RG_KEY_F5 = 294,
	RG_KEY_F6 = 295,
	RG_KEY_F7 = 296,
	RG_KEY_F8 = 297,
	RG_KEY_F9 = 298,
	RG_KEY_F10 = 299,
	RG_KEY_F11 = 300,
	RG_KEY_F12 = 301,
	RG_KEY_F13 = 302,
	RG_KEY_F14 = 303,
	RG_KEY_F15 = 304,
	RG_KEY_F16 = 305,
	RG_KEY_F17 = 306,
	RG_KEY_F18 = 307,
	RG_KEY_F19 = 308,
	RG_KEY_F20 = 309,
	RG_KEY_F21 = 310,
	RG_KEY_F22 = 311,
	RG_KEY_F23 = 312,
	RG_KEY_F24 = 313,
	RG_KEY_F25 = 314,
	RG_KEY_KP_0 = 320,
	RG_KEY_KP_1 = 321,
	RG_KEY_KP_2 = 322,
	RG_KEY_KP_3 = 323,
	RG_KEY_KP_4 = 324,
	RG_KEY_KP_5 = 325,
	RG_KEY_KP_6 = 326,
	RG_KEY_KP_7 = 327,
	RG_KEY_KP_8 = 328,
	RG_KEY_KP_9 = 329,
	RG_KEY_KP_DECIMAL = 330,
	RG_KEY_KP_DIVIDE = 331,
	RG_KEY_KP_MULTIPLY = 332,
	RG_KEY_KP_SUBTRACT = 333,
	RG_KEY_KP_ADD = 334,
	RG_KEY_KP_ENTER = 335,
	RG_KEY_KP_EQUAL = 336,
	RG_KEY_LEFT_SHIFT = 340,
	RG_KEY_LEFT_CONTROL = 341,
	RG_KEY_LEFT_ALT = 342,
	RG_KEY_LEFT_SUPER = 343,
	RG_KEY_RIGHT_SHIFT = 344,
	RG_KEY_RIGHT_CONTROL = 345,
	RG_KEY_RIGHT_ALT = 346,
	RG_KEY_RIGHT_SUPER = 347,
	RG_KEY_MENU = 348,
	RG_KEY_MAX_
};

#endif // RG_WINDOW_H_
