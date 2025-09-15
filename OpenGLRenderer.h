#pragma once
#include "Shader.h"
#include "Shape.h"
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
	OpenGLRenderer();
	~OpenGLRenderer();

	inline GLFWwindow* getWindow() { return window; };
	void setShader(Shader* shader);
	void loadTexture();

	void BindShader();
	void setViewport(uint16_t x, uint16_t y, uint16_t width, uint16_t height);

	int16_t init(uint16_t windowWidth, uint16_t windowHeight);

	void createObjectBuffer(Shape &shape, int32_t index_pointer_size, int32_t normal_pointer_size, float* normals, uint32_t* index_array, std::vector<float> objDataVector);
	uint32_t createTexture(std::string& imagePath);

	void clear();
	void clear(GLuint framebufferID);
	
	void endFrame();
	void render();
	void drawElements(uint32_t ib_size); // temporary to accelerate integration

};

