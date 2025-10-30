#pragma once
#include "Renderer.h"
#include "GLSLShader.h"
#include "Shape.h"
#include "opengl.h"
#include <vector>
#include <map>
#include <string>
class OpenGLRenderer : public Renderer
{
private:
	GLFWwindow *window;
	GLSLShader shader; // one shader for now
	std::unordered_map<uint32_t, uint32_t> typeToUBOMap;
	std::unordered_map<uint32_t, uint32_t> typeToUBOSize;
	std::map<uint32_t, int> shapeVAOIDmap;
	std::map<uint32_t, int> shapeVBOIDmap;
	std::map<uint32_t, int> shapeIBOIDmap;
	std::vector<uint32_t> textures;
	//std::vector<GLuint> framebuffers;
public:
	OpenGLRenderer();
	~OpenGLRenderer();

	int16_t init(uint16_t windowWidth, uint16_t windowHeight) override;

	inline GLFWwindow* getWindow() { return window; };
	void setShader(GLSLShader& shader);
	void initShader(const std::string& path);
	void initShader(const std::string& vertPath, const std::string& fragPath);
	void loadTexture(const std::string &fileName) override;

	void BindShader();
	void BindShape(int shapeType);
	void setViewport(uint16_t x, uint16_t y, uint16_t width, uint16_t height) override;

	void createUBO(uint32_t binding, uint16_t type, uint32_t size);
	void uploadUBOData(uint32_t binding, uint16_t type, uint32_t size, uint32_t offset, void *data);

	void waitIdle() override;

	void createObjectBuffer(Shape &shape, int32_t index_pointer_size, int32_t normal_pointer_size, float* normals, uint32_t* index_array, std::vector<float> objDataVector) override;

	void clear() override;
	void clear(GLuint framebufferID);
	
	void beginFrame() override;
	void endFrame() override;
	void render() override;
	void drawElements(uint32_t ib_size); // temporary to accelerate integration

};

