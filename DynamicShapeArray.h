#pragma once
#include <iostream>
// Include GLEW needs to be before any other openGL includes
#include <GL/glew.h>
// Include GLFW
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include "Shader.h"

#define PI 3.14159265f

#define T_NULL -1
#define T_CUBE 0
#define T_SPHERE 1
#define T_CYLINDER 2
#define T_CIRCLE 3


struct Shape {
	float * data;
	int size;
	int shapeType;
	float speed[3] = { 0.0f ,0.0f ,0.0f };
	float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	unsigned int vao_id = 0;
	unsigned int vb_id = 0;
	unsigned int ib_id = 0;
};

class DynamicShapeArray
{
private:
	Shape* shapeArray;
	int size = 0;
	int capacity;
public:
	DynamicShapeArray();
	~DynamicShapeArray();
	
	void AddArray(float * element, int size, int shapeType);
	void CreateCube(float x0, float y0, float z0, float size);
	float* CreateCircle(float x, float y, float z, float radius);
	void CreateCylinder(float x, float y, float z, float radius, float height);
	void CreateSphere(float radius, int sectorCount, int stackCount);
	void DrawShape(int index, Shader shader);

	void Bindshape(int index);

	
	glm::vec3 * GetCubeNormals(int index, int * n);
	unsigned int GetVAOID(int index);
	unsigned int GetIBOID(int index);
	float * GetShape(int index);
	float * GetSpeed(int index);
	float * GetColor(int index);
	unsigned int GetSize(int index);
	int GetIndexPointerSize(int shapeType);
	unsigned int* GetIndexPointer(int shapeType);
	
	void SetSpeed(int id, float ux, float uy, float uz);
	void SetColor(int id, float r_value, float g_value, float b_value, float alpha_value = 1.0f);

	inline void SetVAOID(int index, int id) { shapeArray[index].vao_id = id; };
	inline void SetBufferID(int index, int id) { shapeArray[index].vb_id = id; };
	inline void SetIBOID(int index, int id) { shapeArray[index].ib_id = id; };
	inline unsigned int GetSize() const { return size; };
private:
	void createBuffer(int shape_index);
	inline int GetShapeType(int shapeIndex) const { return shapeArray[shapeIndex].shapeType; };
	void printData(int index);//might remove that later
	void Extend();
};