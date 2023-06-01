#include <Rogue/Window.h>
#include <Rogue/Core.h>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

const char *RogueGlDebugSourceToString(GLenum source) {
	switch (source) {
	case GL_DEBUG_SOURCE_API: return "OpenGL API";
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "Window system API";
	case GL_DEBUG_SOURCE_SHADER_COMPILER: return "Shader compiler";
	case GL_DEBUG_SOURCE_THIRD_PARTY: return "Third-party";
	case GL_DEBUG_SOURCE_APPLICATION: return "Application";
	case GL_DEBUG_SOURCE_OTHER: return "Other";
	default: return "Unknown";
	}
}

const char *RogueGlDebugSeverityToString(GLenum severity) {
	switch (severity) {
	case GL_DEBUG_SEVERITY_HIGH: return "High";
	case GL_DEBUG_SEVERITY_MEDIUM: return "Medium";
	case GL_DEBUG_SEVERITY_LOW: return "Low";
	case GL_DEBUG_SEVERITY_NOTIFICATION: return "Notification";
	default: return "Unknown";
	}
}

const char *RogueGlDebugMessageTypeToString(GLenum type) {
	switch (type) {
	case GL_DEBUG_TYPE_ERROR: return "Error";
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "Deprecated Behaviour";
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "Undefined Behaviour";
	case GL_DEBUG_TYPE_PORTABILITY: return "Portability";
	case GL_DEBUG_TYPE_PERFORMANCE: return "Performance";
	case GL_DEBUG_TYPE_MARKER: return "Marker";
	case GL_DEBUG_TYPE_PUSH_GROUP: return "Push Group";
	case GL_DEBUG_TYPE_POP_GROUP: return "Pop Group";
	case GL_DEBUG_TYPE_OTHER: return "Other";
	default: return "Unknown";
	}
}

void GLAPIENTRY RogueGlMessageCallback(
	GLenum source,
  GLenum type,
  GLuint id,
  GLenum severity,
  GLsizei length,
  const GLchar *message,
  const void *userParam
) {
  RogueLogError(
		"GL CALLBACK: %s, severity: %s, source: %s\n%s",
		RogueGlDebugMessageTypeToString(type),
		RogueGlDebugSeverityToString(severity),
		RogueGlDebugSourceToString(source),
		message
	);
}

struct RogueWindowImpl {
	GLFWwindow *window;
	GLuint texture;
	GLuint shader;
	GLuint vao;
	GLint scaleUniform, textureUniform;
	RogueKeyState keyStates[ROGUE_KEY_MAX_];
};

void RogueGlfwErrorCallback(int error, const char *message) {
	RogueLogError("GLFW Error (%d): %s", error, message);
}

void RogueGlfwKeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
	RogueWindow *w = glfwGetWindowUserPointer(window);
	static RogueKeyState map[] = {
		[GLFW_PRESS] = ROGUE_KEY_STATE_PRESS,
		[GLFW_RELEASE] = ROGUE_KEY_STATE_RELEASE,
		[GLFW_REPEAT] = ROGUE_KEY_STATE_REPEAT,
	};
	w->impl_->keyStates[key] = map[action];
}

static void RogueWindow_CreateWindow_(RogueWindow *self) {
	glfwSetErrorCallback(&RogueGlfwErrorCallback);
	if (!glfwInit()) RogueFail("Failed to initialize GLFW.");
	self->impl_ = RogueAlloc(sizeof(*self->impl_));
	
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	self->impl_->window = glfwCreateWindow(self->width, self->height, "Rogue", NULL, NULL);
	if (self->impl_->window == NULL) RogueFail("Failed to create GLFW window.");
	glfwSetWindowUserPointer(self->impl_->window, self);
	glfwSetKeyCallback(self->impl_->window, RogueGlfwKeyCallback);

	glfwMakeContextCurrent(self->impl_->window);
	if (gl3wInit() < 0) RogueFail("Failed to initialize GL3W.");

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(&RogueGlMessageCallback, NULL);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
}

static void RogueWindow_CreateTexture_(RogueWindow *self) {
	glCreateTextures(GL_TEXTURE_2D, 1, &self->impl_->texture);
	glTextureParameteri(self->impl_->texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(self->impl_->texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureParameteri(self->impl_->texture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(self->impl_->texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureStorage2D(self->impl_->texture, 1, GL_RGBA8, self->width, self->height);
}

static void RogueWindow_CreateShaderProgram_(RogueWindow *self) {
	self->impl_->shader = glCreateProgram();
	GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);

	{
		static const char *source =
			"#version 460 core\n"
			"out vec2 sUV;\n"
			"void main() {\n"
			"  const vec2 vertices[3] = vec2[3](vec2(-1,-1), vec2(3,-1), vec2(-1, 3));\n"
			"  gl_Position = vec4(vertices[gl_VertexID], 0, 1);\n"
			"  sUV = 0.5 * gl_Position.xy + vec2(0.5); sUV.y = 1.0 - sUV.y;\n"
			"}\n";

		glShaderSource(vShader, 1, &source, NULL);
		glCompileShader(vShader);

		GLint success = GL_FALSE;
		glGetShaderiv(vShader, GL_COMPILE_STATUS, &success);

		if(!success) {
			GLchar message[1024];
			glGetShaderInfoLog(vShader, 1024, NULL, message);
			RogueFail("Failed to compile vertex shader:\n%s", message);
		}
	}
	
	{
		static const char *source =
			"#version 460 core\n"
			"uniform sampler2D uTex;\n"
			"uniform vec2 uScale;\n"
			"in vec2 sUV;\n"
			"out vec4 oColor;\n"
			"void main() {\n"
			"  vec2 uv = vec2(sUV.x / uScale.x, sUV.y / uScale.y);\n"
			"  oColor = texture(uTex, uv);\n"
			"}\n";

		glShaderSource(fShader, 1, &source, NULL);
		glCompileShader(fShader);

		GLint success = GL_FALSE;
		glGetShaderiv(fShader, GL_COMPILE_STATUS, &success);

		if(!success) {
			GLchar message[1024];
			glGetShaderInfoLog(fShader, 1024, NULL, message);
			RogueFail("Failed to compile fragment shader:\n%s", message);
		}
	}

	{
		glAttachShader(self->impl_->shader, vShader);
		glAttachShader(self->impl_->shader, fShader);
		glLinkProgram(self->impl_->shader);
		GLint success = GL_FALSE;
		glGetProgramiv(self->impl_->shader, GL_LINK_STATUS, &success);
		if(success != GL_TRUE) {
			GLsizei log_length = 0;
			GLchar message[1024];
			glGetProgramInfoLog(self->impl_->shader, 1024, &log_length, message);
			RogueFail("Failed to link shader program:\n%s", message);
		}
	}

	glDeleteShader(vShader);
	glDeleteShader(fShader);

	self->impl_->scaleUniform = glGetUniformLocation(self->impl_->shader, "uScale");
	self->impl_->textureUniform = glGetUniformLocation(self->impl_->shader, "uTex");
	glUseProgram(self->impl_->shader);
	glUniform1i(self->impl_->textureUniform, 0);
}

void RogueWindow_CreateVAO_(RogueWindow *self) {
	glCreateVertexArrays(1, &self->impl_->vao);
	glBindVertexArray(self->impl_->vao);
}

void RogueWindow_Init(RogueWindow *self, size_t width, size_t height) {
	self->width = width;
	self->height = height;
	self->buffer = RogueAllocArray(sizeof(*self->buffer), self->width * self->height);
	self->scale.x = 1.0f;
	self->scale.y = 1.0f;

	RogueWindow_CreateWindow_(self);
	RogueWindow_CreateShaderProgram_(self);
	RogueWindow_CreateTexture_(self);
	RogueWindow_CreateVAO_(self);
}

void RogueWindow_DeInit(RogueWindow *self) {
	glDeleteVertexArrays(1, &self->impl_->vao);
	glDeleteTextures(1, &self->impl_->texture);
	glDeleteProgram(self->impl_->texture);

	glfwDestroyWindow(self->impl_->window);

	RogueDeAlloc(self->impl_);
	self->impl_ = NULL;

	glfwTerminate();
	
	RogueDeAlloc(self->buffer);
	self->buffer = NULL;
}

bool RogueWindow_ShouldStop(RogueWindow *self) {
	return glfwWindowShouldClose(self->impl_->window);
}

void RogueWindow_Clear(RogueWindow *self, float r, float g, float b, float a) {
	glClearColor(r, g, b, a);
	glClear(GL_COLOR_BUFFER_BIT);
}

void RogueWindow_Refresh(RogueWindow *self) {
	glTextureSubImage2D(
		self->impl_->texture, 0,
		0, 0, self->width, self->height,
		GL_RGBA, GL_UNSIGNED_BYTE,
		self->buffer
	);

	glUniform2f(self->impl_->scaleUniform, self->scale.x, self->scale.y);
	glBindTextureUnit(0, self->impl_->texture);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	
	for (size_t i = 0; i < ROGUE_KEY_MAX_; ++i)
		self->impl_->keyStates[i] = ROGUE_KEY_STATE_NONE;

	glfwSwapBuffers(self->impl_->window);
	glfwPollEvents();
}

bool RogueWindow_IsKeyDown(RogueWindow *self, RogueKey key) {
	return glfwGetKey(self->impl_->window, key);
}

RogueKeyState RogueWindow_GetKeyState(RogueWindow *self, RogueKey key) {
	return self->impl_->keyStates[key];
}

float RogueWindow_GetTime(RogueWindow *self) {
	return glfwGetTime();
}
