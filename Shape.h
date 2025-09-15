#pragma once
#include <glm/glm.hpp>

struct Shape {
	int size = 0;
	int shapeType = -1;
	float speed[3] = { 0.0f ,0.0f ,0.0f };
	float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	unsigned int vao_id = 0;
	unsigned int vb_id = 0;
	unsigned int ib_id = 0;
	glm::mat4 Model{ 1.f };
	float center[3];
	float d = 0.f;
	float d2 = 0.f;
};
