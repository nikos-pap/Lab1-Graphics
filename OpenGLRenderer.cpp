#include "OpenGLRenderer.h"
#include <iostream>

OpenGLRenderer::OpenGLRenderer() {
	// Initialize OpenGL context here if needed
	window = nullptr;
	shader = nullptr;

}

OpenGLRenderer::~OpenGLRenderer() {
	if (window) glfwDestroyWindow(window);
	glfwTerminate();
}
int16_t OpenGLRenderer::init(uint16_t windowWidth, uint16_t windowHeight) {

	if (!glfwInit()) {
		// raise exception.
		return;
	}


	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(windowWidth, windowHeight, u8"Συγκρουόμενα", NULL, NULL);
	if (!window) {
		glfwTerminate();
		// raise exception.
		return;
	}


	glfwMakeContextCurrent(window);

	// need to do this after valid context 
	if (glewInit() != GLEW_OK)
		std::cout << "Error!" << std::endl;
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	std::cout << glGetString(GL_VERSION) << std::endl;

}
void OpenGLRenderer::clear() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
void OpenGLRenderer::clear(GLuint framebufferID) {
	glBindFramebuffer(GL_FRAMEBUFFER, framebufferID);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLRenderer::render() {
	// TODO: implement the rendering process.
}

void OpenGLRenderer::endFrame() {
	glfwSwapBuffers(window);
	glfwPollEvents();
}

