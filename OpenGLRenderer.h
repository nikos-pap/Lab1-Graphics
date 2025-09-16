#pragma once
#include "GLSLShader.h"
#include "Shape.h"
#include "opengl.h"
#include <vector>
#include <map>
#include <string>
class OpenGLRenderer
{
private:
	GLFWwindow *window;
	GLSLShader *shader; // one shader for now
	std::map<uint32_t, int> shapeVAOIDmap;
	std::map<uint32_t, int> shapeVBOIDmap;
	std::map<uint32_t, int> shapeIBOIDmap;
	std::vector<uint32_t> textures;
	//std::vector<GLuint> framebuffers;
public:
	OpenGLRenderer();
	~OpenGLRenderer();

	inline GLFWwindow* getWindow() { return window; };
	void setShader(GLSLShader* shader);
	void loadTexture(std::string fileName);

	void BindShader();
	void BindShape(int shapeType);
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

