#include "OpenGLRenderer.h"
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION   
#include "stb_image.h"

#ifdef _DEBUG
void APIENTRY glDebugOutput(GLenum source,
	GLenum type,
	unsigned int id,
	GLenum severity,
	GLsizei length,
	const char* message,
	const void* userParam);
#endif
OpenGLRenderer::OpenGLRenderer() {
	// Initialize OpenGL context here if needed
	window = nullptr;
}

OpenGLRenderer::~OpenGLRenderer() {
	if (window) glfwDestroyWindow(window);
	glfwTerminate();
}

//loads texture from file
void OpenGLRenderer::loadTexture(const std::string &fileName)
{
	uint32_t texture;
	glGenTextures(1, &texture);

	//generating cube map
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

	//Define all 6 faces
	// load and generate the texture
	int width, height, nrChannels;
	unsigned char* data = stbi_load(fileName.c_str(), &width, &height, &nrChannels, STBI_rgb_alpha);
	if (data)
	{
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 0);

	stbi_image_free(data);
	textures.push_back(texture);
}
int16_t OpenGLRenderer::init(uint16_t windowWidth, uint16_t windowHeight) {

	if (!glfwInit()) {
		// raise exception.
		return -1;
	}


	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(windowWidth, windowHeight, "Shape Rammer 3000", NULL, NULL);
	if (!window) {
		glfwTerminate();
		// raise exception.
		return -1;
	}

	glfwMakeContextCurrent(window);

	// need to do this after valid context 
	if (glewInit() != GLEW_OK) {
		std::cout << "Error!" << std::endl;
		return -1;
	}
	int flags;
	glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
#ifdef _DEBUG
	std::cout << "Debug context enabled" << std::endl;
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
	{
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(glDebugOutput, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
	}

#endif

	std::cout << glGetString(GL_VERSION) << std::endl;

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_BLEND);
	glEnable(GL_NORMALIZE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//for the textures
	glDisable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	return 1;

}
void OpenGLRenderer::BindShape(int shapeType) {
	if (shapeVAOIDmap.find(shapeType) != shapeVAOIDmap.end()) {
		glBindVertexArray(shapeVAOIDmap[shapeType]);
	}
	if (shapeIBOIDmap.find(shapeType) != shapeIBOIDmap.end()) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shapeIBOIDmap[shapeType]);
	}
}
void OpenGLRenderer::createUBO(uint32_t binding, uint16_t type, uint32_t size) {
	GLuint ubo;
	glGenBuffers(1, &ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferData(GL_UNIFORM_BUFFER, size, NULL, GL_STATIC_DRAW);
	glBindBufferRange(GL_UNIFORM_BUFFER, binding, ubo, 0, size);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	typeToUBOMap[type] = ubo;
	typeToUBOSize[type] = size;
}
void OpenGLRenderer::waitIdle() {
	// do nothing
	BindShader();
}

void OpenGLRenderer::uploadUBOData(uint32_t binding, uint16_t type, uint32_t size, uint32_t offset, void *data) {
	glBindBuffer(GL_UNIFORM_BUFFER, typeToUBOMap[type]);
	glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
	glBindBufferRange(GL_UNIFORM_BUFFER, binding, typeToUBOMap[type], 0, typeToUBOSize[type]);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void OpenGLRenderer::createObjectBuffer(Shape &shape, int32_t index_pointer_size, int32_t normal_pointer_size, float *normals, uint32_t *index_array, std::vector<float> objDataVector) {
	unsigned int vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	unsigned int buffer_id;

	glGenBuffers(1, &buffer_id);
	glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
	glBufferData(GL_ARRAY_BUFFER, shape.size * sizeof(float) + normal_pointer_size * sizeof(float), 0, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, shape.size * sizeof(float), objDataVector.data());
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);

	glBufferSubData(GL_ARRAY_BUFFER, shape.size * sizeof(float), normal_pointer_size * sizeof(float), normals);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)(shape.size * sizeof(float)));

	//create a buffer for the indices
	unsigned int ibo;
	//glGenBuffers creates the random id for that buffer and stores it in the variable
	glGenBuffers(1, &ibo);
	//bind object buffer to target
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_pointer_size * sizeof(unsigned int), index_array, GL_STATIC_DRAW);

	shapeVAOIDmap[shape.shapeType] = vao;
	shapeVBOIDmap[shape.shapeType] = buffer_id;
	shapeIBOIDmap[shape.shapeType] = ibo;
	std::cout << "buffer created id's are:" << vao << ", " << buffer_id << ", " << ibo << std::endl;
}
void OpenGLRenderer::clear() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
void OpenGLRenderer::clear(GLuint framebufferID) {
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferID);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLRenderer::render() {
	// TODO: implement the complete rendering process.
}
void OpenGLRenderer::drawElements(uint32_t ib_size) {
	glDrawElements(GL_TRIANGLES, ib_size, GL_UNSIGNED_INT, nullptr);
}
void OpenGLRenderer::initShader(const std::string& path) {
	GLSLShader aShader{ path };
	shader = aShader;
	shader.Bind();
}
void OpenGLRenderer::initShader(const std::string& vertPath, const std::string& fragPath) {
	GLSLShader aShader{ vertPath, fragPath };
	shader = aShader;
	shader.Bind();
}
void OpenGLRenderer::setShader(GLSLShader &shader) {
	this->shader = shader;
	shader.Bind();
}
void OpenGLRenderer::setViewport(uint16_t x, uint16_t y, uint16_t width, uint16_t height) {
	glViewport(x, y, width, height);
}
void OpenGLRenderer::BindShader() {
	shader.Bind();
}

void OpenGLRenderer::beginFrame() {
	clear();
}
void OpenGLRenderer::endFrame() {
	glfwSwapBuffers(window);
	glfwPollEvents();
}

#ifdef _DEBUG
void APIENTRY glDebugOutput(GLenum source,
	GLenum type,
	unsigned int id,
	GLenum severity,
	GLsizei length,
	const char* message,
	const void* userParam)
{
	// ignore non-significant error/warning codes
	if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

	std::cout << "---------------" << std::endl;
	std::cout << "Debug message (" << id << "): " << message << std::endl;

	switch (source)
	{
	case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
	case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
	case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
	} std::cout << std::endl;

	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break;
	case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
	case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
	case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
	case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
	case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
	case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
	} std::cout << std::endl;

	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
	case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
	case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
	case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
	} std::cout << std::endl;
	std::cout << std::endl;
}
#endif
