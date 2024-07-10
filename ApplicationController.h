#pragma once
#include "InputController.h"
#include "Shader.h"

#ifdef _WIN32
#include <Windows.h>
#endif


class ApplicationController {
private:
	GLFWwindow* window;
	CameraController* camera;
	DynamicShapeArray* shapeArray;
	InputController* inputController;
	unsigned int loadTexture();
public:
	ApplicationController();
	int start();
};