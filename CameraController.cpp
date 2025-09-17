#include "CameraController.h"

CameraController::CameraController() {
	position = glm::vec3(-100.0f, 150.0f, 150.0f);
	forward = glm::vec3(2.0f, -1.5f, -1.5f);
	up = glm::vec3(0.0f, 1.0f, 0.0f);
	updateView();
}

CameraController::CameraController(glm::vec3 position, glm::vec3 forward, glm::vec3 up) {
	this->position = position;
	this->forward = forward;
	this->up = up;
	updateView();
}

CameraController::~CameraController() {

}

glm::vec3 &CameraController::getPosition() {
	return position;
}

glm::vec3 CameraController::getForward() {
	return forward;
}

glm::vec3 CameraController::getUp() {
	return up;
}

glm::mat4 CameraController::getView() {
	return view;
}

void CameraController::updateView() {
	view = glm::lookAt(this->position, (this->position + this->forward), this->up);
}

void CameraController::moveZ(float speed) {
	position += speed * forward;
}

void CameraController::moveX(float speed) {
	position += glm::normalize(glm::cross(forward, up)) * speed;
}

void CameraController::moveY(float speed) {
	position -= speed * up;
}

void CameraController::panX(float speed) {
	forward += glm::normalize(glm::cross(forward, up)) * speed;
}

void CameraController::panY(float speed) {
	forward -= speed * up;
}
