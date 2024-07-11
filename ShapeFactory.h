#include "opengl.h"
#include <cstdlib>
#include <chrono>
#include <random>
#include <iostream>

#define PI 3.14159265f
#define SQRT_2 1.41421356237f
#define T_CUBE 0
#define T_SPHERE 1
#define T_CYLINDER 2
#define T_RING 3

#define CIRCLE_VERTEX_NUM (CIRCLE_TRIANGLE_NUM+2)

#define SPHERE_SECTOR_NUM 36
#define SPHERE_STACK_NUM 18
#define CIRCLE_TRIANGLE_NUM 34


struct Shape {
	float * data;
	int size;
	int shapeType;
	float speed[3] = { 0.0f ,0.0f ,0.0f };
	float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	unsigned int vao_id = 0;
	unsigned int vb_id = 0;
	unsigned int ib_id = 0;
	glm::mat4 Model;
	float center[3];
	float d;
	float d2;
};

class ShapeFactory {
private:



	//Normals
/*
	Indices for cube triangle points have been numbered in the following way on the 2 faces back and front(+4)
	1 - 3 5 - 7
	| X | | X |
	0 - 2 4 - 6
	back: 0123, front: 4567, left: 0145, right: 2367, bottom: 4062, top: 5173
	The only easy enough to do by hand
*/
	float cube_normals[24];

	float sphere_normals[2109];
	float cylinder_normals[216];
	float ring_normals[8 * (CIRCLE_VERTEX_NUM - 1) * 3];

	//indice arrays
	unsigned int cube_indices[36];


	unsigned int cylinder_indices[CIRCLE_TRIANGLE_NUM * 3 * 4];
	unsigned int sphere_indices[2 * 3 * (SPHERE_STACK_NUM - 1) * SPHERE_SECTOR_NUM];
	unsigned int  ring_indices[2 * 8 * CIRCLE_TRIANGLE_NUM * 3];

	//flags
	bool firstCylinder = true;
	bool firstRing = true;
	bool firstSphere = true;


	void createBuffer(Shape& shape, int index);

	unsigned int* GetIndexPointer(Shape shape, int index);


	void InitSphereIndices();
	void InitCylinderIndices();
	void AddCircleIndices(unsigned int* indices, int index, int offset = 0);

	Shape CreateShapeObject(float* element, int elementSize, int shapeType, float x0, float y0, float z0, float d, int size);
	
	//functions that create shapes
	float* CreateCircle(float x, float y, float z, float radius);
	Shape CreateCube(float x0, float y0, float z0, float size, int shapeArraySize);
	Shape CreateSphere(float x0, float y0, float z0, float radius, int shapeArraySize);
	Shape CreateCylinder(float x, float y, float z, float radius, float height, int shapeArraySize);
	Shape CreateRing(float x0, float y0, float z0, float r1, float r2, int shapeArraySize);
	

	int RandomInt(int min, int max);
	float RandomFloat(float min, float max);

public:
	ShapeFactory();

	int GetIndexPointerSize(int shapeType);
	float* GetNormals(int shapeType);

	//creates Shapes and adds them to the Array
	Shape CreateRandomShape(int shapeArraySize);
	Shape CreateShape(float x, float y, float z, int size, int ShapeType, int shapeArraySize);

	// Color handlers
	void SetRandomColor(Shape& shape, float alpha_value = 1.0f);
	float* GetColor(Shape& shape);
	void SetColor(Shape& shape, float r_value, float g_value, float b_value, float alpha_value);
	
};