#include <cstdlib>
#include <iostream>
#include "DynamicShapeArray.h"
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

unsigned int cube_indices2[] = {
		4, 5, 6,//front
		5, 6, 7,//front
		0, 1, 2,//back
		1, 2, 3,//back
		0, 1, 4,//left
		1, 4, 5,//left
		2, 3, 6,//right
		3, 6, 7,//right
		5, 1, 7,//top
		1, 7, 3,//top
		4, 0, 6,//bottom
		0, 6, 2//bottom
};

DynamicShapeArray::DynamicShapeArray()
{
	capacity = 10;
	shapeArray = (Shape *) malloc(capacity * sizeof(Shape));
	size = 0;
}

DynamicShapeArray::~DynamicShapeArray()
{
	free(shapeArray);
}

void DynamicShapeArray::AddArray(float * element, int elementSize, int shapeType)
{
	if (capacity == size) {
		Extend();
	}

	float * tmpData = (float*)malloc(elementSize * sizeof(float));
	if (tmpData != nullptr) {
		for (int i = 0; i < elementSize; i++) {
			tmpData[i] = element[i];
		}
		shapeArray[size++] = { tmpData, elementSize, shapeType };
	} else {
		std::cout << "not ok" << std::endl;
	}
}

void DynamicShapeArray::CreateCube(float x0, float y0, float z0, float size) {
	float x1 = x0 + size;
	float y1 = y0 + size;
	float z1 = z0 + size;
	float positions[] = {
		x0, y0, z0,//00  back(0)0
		x0, y1, z0,//01 back(0)1
		x1, y0, z0,//10 back(0)2
		x1, y1, z0,//11 back(0)3
		x0, y0, z1,//00 front(1)4
		x0, y1, z1,//01 front(1)5
		x1, y0, z1,//10 front(1)6
		x1, y1, z1,//11 front(1)7
	};
	AddArray(positions, 8 * 3, T_CUBE);
}

void DynamicShapeArray::printData(int id) {
	for (int shape = 0; shape < 24; shape++) {
		std::cout << shapeArray[id].data[shape] << std::endl;
	}
}

void DynamicShapeArray::GetNormals(int id)
{
	for (int shape = 0, n=0; shape < 36;) {
		unsigned int p1_i = 3*cube_indices2[shape++];
		unsigned int p2_i = 3*cube_indices2[shape++];
		unsigned int p3_i = 3*cube_indices2[shape++];
		glm::vec3 p1 = glm::vec3(shapeArray[id].data[p1_i], shapeArray[id].data[p1_i + 1], shapeArray[id].data[p1_i + 2]);
		glm::vec3 p2 = glm::vec3(shapeArray[id].data[p2_i], shapeArray[id].data[p2_i + 1], shapeArray[id].data[p2_i + 2]);
		glm::vec3 p3 = glm::vec3(shapeArray[id].data[p3_i], shapeArray[id].data[p3_i + 1], shapeArray[id].data[p3_i + 2]);
		glm::vec3 u,v,N;
		u = p2 - p1;
		v = p3 - p1;
		N = normalize(cross(u, v));
	}
}

float * DynamicShapeArray::GetShape(int index)
{
	if (index < size) {
		if (shapeArray[index].shapeType != T_NULL) {
			return shapeArray[index].data;
		}
	}
	return nullptr;
}

float * DynamicShapeArray::GetSpeed(int id)
{
	if (id < size) {
		return shapeArray[id].speed;
	}
	return nullptr;
}

float* DynamicShapeArray::GetColor(int id)
{
	if (id < size) {
		return shapeArray[id].color;
	}
	return nullptr;
}

unsigned int DynamicShapeArray::GetSize(int index)
{
	if (index < size) {
		return shapeArray[index].size;
	}
	return -1;
}

void DynamicShapeArray::SetColor(int id, float r_value, float g_value, float b_value, float alpha_value)
{
	if (id < size) {
		shapeArray[id].color[0] = r_value;
		shapeArray[id].color[1] = g_value;
		shapeArray[id].color[2] = b_value;
		shapeArray[id].color[3] = alpha_value;
	}
}

void DynamicShapeArray::SetSpeed(int id, float ux, float uy, float uz)
{
	if (id < size) {
		shapeArray[id].speed[0] = ux; 
		shapeArray[id].speed[1] = uy; 
		shapeArray[id].speed[2] = uz;
	}
}

void DynamicShapeArray::Extend()
{
	capacity += 10;
	Shape * temp = (Shape*) realloc(shapeArray, (capacity) * sizeof(Shape));
	if (temp != nullptr) {
		for (int i = 0; i < size; i++) {
			temp[i] = shapeArray[i];
		}
		shapeArray = temp;
	}
}

