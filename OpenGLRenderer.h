#pragma once
#include "Shader.h"
class OpenGLRenderer
{
private:
	Shader shader;
public:
	OpenGLRenderer();
	~OpenGLRenderer();
	void loadTexture();

};

