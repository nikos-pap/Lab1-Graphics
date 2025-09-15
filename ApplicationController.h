#pragma once
#include "InputController.h"
#include "Shader.h"
#include "Renderer.h"
#include "OpenGLRenderer.h"

#ifdef _WIN32
#include <Windows.h>
#endif


class ApplicationController {
private:
	GLFWwindow* window;
	CameraController* camera;
	DynamicShapeArray* shapeArray;
	InputController* inputController;
	OpenGLRenderer* renderer; // TODO: change this to Renderer* when other renderers are implemented
	unsigned int loadTexture();
public:
	ApplicationController();
	int start();
};