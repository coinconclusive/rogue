#include <Rogue/Window.h>
#include <Rogue/Core.h>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

const char *RgGlDebugSourceToString(GLenum source) {
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

const char *RgGlDebugSeverityToString(GLenum severity) {
	switch (severity) {
	case GL_DEBUG_SEVERITY_HIGH: return "High";
	case GL_DEBUG_SEVERITY_MEDIUM: return "Medium";
	case GL_DEBUG_SEVERITY_LOW: return "Low";
	case GL_DEBUG_SEVERITY_NOTIFICATION: return "Notification";
	default: return "Unknown";
	}
}

const char *RgGlDebugMessageTypeToString(GLenum type) {
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

void GLAPIENTRY RgGlMessageCallback(
	GLenum source,
  GLenum type,
  GLuint id,
  GLenum severity,
  GLsizei length,
  const GLchar *message,
  const void *userParam
) {
  RgLogError(
		"GL CALLBACK: %s, severity: %s, source: %s\n%s",
		RgGlDebugMessageTypeToString(type),
		RgGlDebugSeverityToString(severity),
		RgGlDebugSourceToString(source),
		message
	);
}

struct RgWindowImpl {
	GLFWwindow *window;
	GLuint texture;
	GLuint shader;
	GLuint vao;
	GLint scaleUniform, textureUniform;
	RgKeyState *keyStates;
};

void RgGlfwErrorCallback(int error, const char *message) {
	RgLogError("GLFW Error (%d): %s", error, message);
}

void RgGlfwKeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
	RgWindow *self = glfwGetWindowUserPointer(window);

	static RgKeyState map[] = {
		[GLFW_PRESS] = RG_KEY_STATE_PRESS,
		[GLFW_RELEASE] = RG_KEY_STATE_RELEASE,
		[GLFW_REPEAT] = RG_KEY_STATE_REPEAT,
	};

	self->impl_->keyStates[key] = map[action];
}

void RgGlfwWindowSizeCallback(GLFWwindow *window, int newWidth, int newHeight);

static void RgWindow_CreateWindow_(RgWindow *self) {
	glfwSetErrorCallback(&RgGlfwErrorCallback);
	if (!glfwInit()) RgFail("Failed to initialize GLFW.");
	
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	self->impl_->window = glfwCreateWindow(self->width, self->height, "Rg", NULL, NULL);
	if (self->impl_->window == NULL) RgFail("Failed to create GLFW window.");

	glfwSetWindowUserPointer(self->impl_->window, self);

	glfwSetKeyCallback(self->impl_->window, &RgGlfwKeyCallback);
	glfwSetWindowSizeCallback(self->impl_->window, &RgGlfwWindowSizeCallback);

	glfwMakeContextCurrent(self->impl_->window);
	if (gl3wInit() < 0) RgFail("Failed to initialize GL3W.");

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(&RgGlMessageCallback, NULL);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
}

static void RgWindow_CreateTexture_(RgWindow *self) {
	glCreateTextures(GL_TEXTURE_2D, 1, &self->impl_->texture);
	glTextureParameteri(self->impl_->texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(self->impl_->texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureParameteri(self->impl_->texture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(self->impl_->texture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTextureStorage2D(self->impl_->texture, 1, GL_RGBA8, self->bufferWidth, self->bufferHeight);
}

static void RgWindow_CreateShaderProgram_(RgWindow *self) {
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
			RgFail("Failed to compile vertex shader:\n%s", message);
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
			RgFail("Failed to compile fragment shader:\n%s", message);
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
			RgFail("Failed to link shader program:\n%s", message);
		}
	}

	glDeleteShader(vShader);
	glDeleteShader(fShader);

	self->impl_->scaleUniform = glGetUniformLocation(self->impl_->shader, "uScale");
	self->impl_->textureUniform = glGetUniformLocation(self->impl_->shader, "uTex");
	glUseProgram(self->impl_->shader);
	glUniform1i(self->impl_->textureUniform, 0);
}

void RgWindow_CreateVAO_(RgWindow *self) {
	glCreateVertexArrays(1, &self->impl_->vao);
	glBindVertexArray(self->impl_->vao);
}

void RgWindow_CreateBuffer_(RgWindow *self) {
	self->bufferWidth = self->width / self->scale.x;
	self->bufferHeight = self->height / self->scale.y;
	self->buffer = RgAllocArray(sizeof(*self->buffer), self->bufferWidth * self->bufferHeight);
}

void RgGlfwWindowSizeCallback(GLFWwindow *window, int newWidth, int newHeight) {
	RgWindow *self = glfwGetWindowUserPointer(window);

	if (self->width == newWidth && self->height == newHeight) return;

	self->width = newWidth;
	self->height = newHeight;
	
	RgDeAlloc(self->buffer);
	RgWindow_CreateBuffer_(self);

	glDeleteTextures(1, &self->impl_->texture);
	RgWindow_CreateTexture_(self);
}

void RgWindow_Init(RgWindow *self, const RgWindowInitInfo *info) {
	self->width = info->width;
	self->height = info->height;
	self->scale.x = info->scaleX;
	self->scale.y = info->scaleY;
	self->buffer = NULL;
	self->impl_ = RgAlloc(sizeof(*self->impl_));
	self->impl_->keyStates = RgAllocArray(sizeof(*self->impl_->keyStates), RG_KEY_MAX_ + 1);

	RgWindow_CreateBuffer_(self);
	RgWindow_CreateWindow_(self);
	RgWindow_CreateShaderProgram_(self);
	RgWindow_CreateTexture_(self);
	RgWindow_CreateVAO_(self);
}

void RgWindow_DeInit(RgWindow *self) {
	glDeleteVertexArrays(1, &self->impl_->vao);
	glDeleteTextures(1, &self->impl_->texture);
	glDeleteProgram(self->impl_->shader);

	glfwDestroyWindow(self->impl_->window);

	RgDeAlloc(self->impl_->keyStates);
	self->impl_->keyStates = NULL;

	RgDeAlloc(self->impl_);
	self->impl_ = NULL;

	glfwTerminate();
	
	RgDeAlloc(self->buffer);
	self->buffer = NULL;
}

bool RgWindow_ShouldStop(RgWindow *self) {
	return glfwWindowShouldClose(self->impl_->window);
}

void RgWindow_Clear(RgWindow *self, float r, float g, float b, float a) {
	glClearColor(r, g, b, a);
	glClear(GL_COLOR_BUFFER_BIT);
}

void RgWindow_Refresh(RgWindow *self) {
	glTextureSubImage2D(
		self->impl_->texture, 0,
		0, 0, self->bufferWidth, self->bufferHeight,
		GL_RGBA, GL_UNSIGNED_BYTE,
		self->buffer
	);

	glUniform2f(self->impl_->scaleUniform, 1.0f, 1.0f); // don't scale twice!
	glBindTextureUnit(0, self->impl_->texture);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	
	for (size_t i = 0; i < RG_KEY_MAX_; ++i)
		self->impl_->keyStates[i] = RG_KEY_STATE_NONE;

	glfwSwapBuffers(self->impl_->window);
	glfwPollEvents();

	int width, height;
	glfwGetFramebufferSize(self->impl_->window, &width, &height);
	glViewport(0, 0, width, height);
}

bool RgWindow_IsKeyDown(RgWindow *self, RgKey key) {
	return glfwGetKey(self->impl_->window, key);
}

RgKeyState RgWindow_GetKeyState(RgWindow *self, RgKey key) {
	return self->impl_->keyStates[key];
}

float RgWindow_GetTime(RgWindow *self) {
	return glfwGetTime();
}
