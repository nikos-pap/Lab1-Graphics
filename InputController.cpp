#include "InputController.h"
#include <iostream>

bool tex = true;

InputController::InputController(CameraController* camera, DynamicShapeArray* shapeArray) {
	this->camera = camera;
	this->shapeArray = shapeArray;
}


int InputController::parseInputs(GLFWwindow* window) {
	int present = glfwJoystickPresent(GLFW_JOYSTICK_1);
	if (1 == present) {
		int axisCount;
		const float* axis = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axisCount);

		int buttonCount;
		const unsigned char* buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttonCount);
		if (GLFW_PRESS == buttons[1] && spaceChecker) {
			joystick_space = true;
			spaceChecker = false;
			shapeArray->CreateRandomShape();
		}
		else if (GLFW_RELEASE == buttons[1] && joystick_space) {
			joystick_space = false;
			spaceChecker = true;
		}

		if (GLFW_PRESS == buttons[0]) {
			return GLFW_PRESS;
		}

		const char* name = glfwGetJoystickName(GLFW_JOYSTICK_1);

		if (axis[5] >= -1.0) //R2
			camera->moveZ(axis[5] + 1);
		if (abs(axis[1]) >= 0.2)//LY
			camera->moveY(axis[1]);
		if (axis[4] >= -1.0)//L2
			camera->moveZ(-axis[4] - 1);
		if (abs(axis[0]) >= 0.2) //LX
			camera->moveX(axis[0]);
		if (abs(axis[3]) >= 0.2) //RY
			camera->panY(axis[3] / 30);
		if (abs(axis[2]) >= 0.2) //RX
			camera->panX(axis[2] / 30);

		if (buttons[11] == GLFW_PRESS)//right arrow
			shapeArray->MoveSphere(1, glm::vec3(1.0f, 0.0f, 0.0f));
		if (buttons[13] == GLFW_PRESS)//left arrow
			shapeArray->MoveSphere(1, glm::vec3(-1.0f, 0.0f, 0.0f));
		if (buttons[10] == GLFW_PRESS)//up arrow
			shapeArray->MoveSphere(1, glm::vec3(0.0f, 1.0f, 0.0f));
		if (buttons[12] == GLFW_PRESS)//down arrow
			shapeArray->MoveSphere(1, glm::vec3(0.0f, -1.0f, 0.0f));
		if (buttons[4] == GLFW_PRESS)//L1 arrow
			shapeArray->MoveSphere(1, glm::vec3(0.0f, 0.0f, 1.0f));
		if (buttons[5] == GLFW_PRESS)//R1 arrow
			shapeArray->MoveSphere(1, glm::vec3(0.0f, 0.0f, -1.0f));
		if (buttons[6] == GLFW_PRESS)//Select
			shapeArray->SpeedUP(false);
		if (buttons[7] == GLFW_PRESS)//Start
			shapeArray->SpeedUP(true);
		if (buttons[2] == GLFW_PRESS && texChecker) {//X
			texChecker = false;
			joystick_tex = true;
			tex = !tex;
		}
		else if (buttons[2] == GLFW_RELEASE && joystick_tex) {
			joystick_tex = false;
			texChecker = true;
		}

		//stop bounce sound
		if (buttons[3] == GLFW_PRESS && muteChecker) {
			muteChecker = false;
			soundsEnabled = !soundsEnabled;
			joystick_mute = true;
		}
		else if (buttons[3] == GLFW_RELEASE && joystick_mute) {
			muteChecker = true;
			joystick_mute = false;
		}
	}


	if ((glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) && spaceChecker && !joystick_space) {
		spaceChecker = false;
		shapeArray->CreateRandomShape();
	}
	else if ((glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) && !joystick_space) {
		spaceChecker = true;
	}
	if ((glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) && texChecker && !joystick_tex) {
		texChecker = false;
		tex = !tex;
	}
	else if ((glfwGetKey(window, GLFW_KEY_T) == GLFW_RELEASE) && !joystick_tex) {
		texChecker = true;
	}

	//move camera
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera->moveZ(cameraSpeed + 1);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera->moveZ(-cameraSpeed - 1);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera->moveX(-cameraSpeed);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera->moveX(cameraSpeed);
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		camera->moveY(-cameraSpeed);
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
		camera->moveY(cameraSpeed);

	//camera controls
	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
		camera->panY(-yawSpeed);
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
		camera->panY(yawSpeed);
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
		camera->panX(-yawSpeed / 2);
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
		camera->panX(yawSpeed / 2);

	//Sphere Controls
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		shapeArray->MoveSphere(1, glm::vec3(1.0f, 0.0f, 0.0f));
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		shapeArray->MoveSphere(1, glm::vec3(-1.0f, 0.0f, 0.0f));
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		shapeArray->MoveSphere(1, glm::vec3(0.0f, 1.0f, 0.0f));
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		shapeArray->MoveSphere(1, glm::vec3(0.0f, -1.0f, 0.0f));
	if (glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS)
		shapeArray->MoveSphere(1, glm::vec3(0.0f, 0.0f, -1.0f));
	if (glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS)
		shapeArray->MoveSphere(1, glm::vec3(0.0f, 0.0f, 1.0f));

	//speed UP/DOWN
	if ((glfwGetKey(window, GLFW_KEY_PERIOD) == GLFW_PRESS)) {
		shapeArray->SpeedUP(true);
	}
	if ((glfwGetKey(window, GLFW_KEY_COMMA) == GLFW_PRESS)) {
		shapeArray->SpeedUP(false);
	}

	//stop bounce sound
	if ((glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) && muteChecker && !joystick_mute) {
		muteChecker = false;
		soundsEnabled = !soundsEnabled;
	}
	else if ((glfwGetKey(window, GLFW_KEY_M) == GLFW_RELEASE) && !joystick_mute) {
		muteChecker = true;
	}

	camera->updateView();

	return glfwGetKey(window, GLFW_KEY_ESCAPE);
}