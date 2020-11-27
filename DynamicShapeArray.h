#pragma once
#include <iostream>
// Include GLEW needs to be before any other openGL includes
#include <GL/glew.h>
// Include GLFW
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

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

#define GLOBAL_SPEED 60
#define MAX_SPEEDUP 100


extern bool soundsEnabled;
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

class DynamicShapeArray
{
public:
	DynamicShapeArray();
	~DynamicShapeArray();

	//creates Shapes and adds them to the Array
	void CreateShape(float x, float y, float z, int size, int ShapeType);
	void CreateRandomShape();
	
	//Binds VAO and ib of the shape at the index
	void BindShape(int index);

	//Getters
	//Returns the color of the shape to pass into the shader
	float * GetColor(int index);
	
	//Returns the size of the ShapeArray
	inline int GetSize() { return size; };
	glm::mat4 GetModel(int index);
	int GetIndexPointerSize(int shapeType);//Returns the size of the ib to use when drawing

	//movement
	void SpeedUP(bool up);
	void Move(int index);
	void MoveSphere(int index, glm::vec3 speed);
	
	//Setters
	void SetColor(int id, float r_value, float g_value, float b_value, float alpha_value = 1.0f);
	void SetRandomColor(int index, float alpha_value = 1.0f);

private:
	Shape* shapeArray;
	int size = 0;
	int capacity;
	
	void CheckCollision(int index);
	void Collide(int index1, int index2);
	
	float * GetNormals(int shapeType);
	
	//functions that create shapes
	void CreateCube(float x0, float y0, float z0, float size);
	void CreateCylinder(float x, float y, float z, float radius, float height);
	float* CreateCircle(float x, float y, float z, float radius);
	void CreateSphere(float x0, float y0, float z0, float radius);
	void CreateRing(float x0, float y0, float z0, float r1, float r2);
	
	void createBuffer(int index);

	//functions that initialize index arrays they run when shapeArray is initialized
	void InitCylinderIndices();
	void AddCircleIndices(unsigned int* indices, int index, int offset = 0);
	void InitSphereIndices();

	//Extends the Array used when Array size reaches its capacity
	void Extend();

	//assisting function
	unsigned int* GetIndexPointer(int index);
	void AddArray(float* element, int size, int shapeType, float x0, float y0, float z0, float d);//helps with shape creation
	
	//Random functions
	int RandomInt(int min, int max);
	float RandomFloat(float min, float max);
};