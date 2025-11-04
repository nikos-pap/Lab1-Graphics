#pragma once
#include <glm/glm.hpp>
//#define T_CUBE 0
//#define T_SPHERE 1
//#define T_CYLINDER 2
//#define T_RING 3
enum ShapeType {
	T_CUBE = 0,
	T_SPHERE,
	T_CYLINDER,
	T_RING
};

struct objMatrices {
	glm::mat4 mvp{ 1.f };
	glm::mat4 model{ 1.f };
	glm::mat3 normalModel{ 1.f };
};

struct Shape {
	int size = 0;
	int shapeType = -1;
	float speed[3] = { 0.0f ,0.0f ,0.0f };
	float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float center[3];
	objMatrices matrices;
	float d = 0.f;
	float d2 = 0.f;
};
