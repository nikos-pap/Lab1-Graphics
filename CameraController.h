#pragma once
#include "opengl.h"

class CameraController {
private:
	glm::vec3 position;
	glm::vec3 forward;
	glm::vec3 up;
	glm::mat4 view;

public:
	CameraController();
	CameraController(glm::vec3 position, glm::vec3 forward, glm::vec3 up);
	~CameraController();

	void moveX(float speed);
	void moveY(float speed);
	void moveZ(float speed);

	void panY(float speed);
	void panX(float speed);

	glm::vec3 &getPosition();
	glm::vec3 getForward();
	glm::vec3 getUp();
	glm::mat4 getView();
	void updateView();
};