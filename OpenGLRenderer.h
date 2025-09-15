#pragma once
#include "Shader.h"
#include "opengl.h"
#include <vector>
#include <string>
class OpenGLRenderer
{
private:
	GLFWwindow *window;
	Shader *shader; // one shader for now
	std::vector<GLuint> textures;
	//std::vector<GLuint> framebuffers;
public:
	inline GLFWwindow* getWindow() { return window; };
	int16_t init(uint16_t windowWidth, uint16_t windowHeight);
	uint32_t createTexture(std::string& imagePath);
	void clear();
	void clear(GLuint framebufferID);
	void endFrame();
	void render();
	OpenGLRenderer();
	~OpenGLRenderer();
	void loadTexture();

};

