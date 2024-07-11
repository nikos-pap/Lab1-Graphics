#pragma once
#include "ShapeFactory.h"
/*
	Deprecated. Added to ShapeFactory.
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
*/
#define GLOBAL_SPEED 60
#define MAX_SPEEDUP 100


extern bool soundsEnabled;
/*
	Deprecated. Added to ShapeFactory.
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
};*/

class DynamicShapeArray
{
public:
	DynamicShapeArray();
	~DynamicShapeArray();

	//creates Shapes and adds them to the Array
	void CreateRandomShape();
	void CreateShape(float x, float y, float z, int size, int ShapeType);
	
	//Binds VAO and ib of the shape at the index
	void BindShape(int index);

	//movement
	void Move(int index);
	void MoveSphere(int index, glm::vec3 speed);
	void SpeedUP(bool up);

	//Getters
	inline int GetSize() { return size; };//Returns the size of the ShapeArray
	inline glm::mat4 GetModel(int index) { return shapeArray[index].Model; };
	float * GetColor(int index);//Returns the color of the shape to pass into the shader
	int GetIndexPointerSize(int index);//Returns the size of the ib to use when drawing

	//Setters
	void SetColor(int index, float r_value, float g_value, float b_value, float alpha_value = 1.0f);
	void SetRandomColor(int index, float alpha_value = 1.0f);


private:
	Shape* shapeArray;
	ShapeFactory* shapeFactory;
	int size;
	int capacity;
	
	//collision handling
	void CheckCollision(int index);
	void Collide(int index1, int index2);
	
	
	//functions that create shapes
	//void CreateCube(float x0, float y0, float z0, float size);
	//void CreateSphere(float x0, float y0, float z0, float radius);
	//void CreateCylinder(float x, float y, float z, float radius, float height);
	//void CreateRing(float x0, float y0, float z0, float r1, float r2);
	//float* CreateCircle(float x, float y, float z, float radius);
	
	//handles buffer creation and data
	//void createBuffer(int index);

	//functions that initialize index arrays they run when shapeArray is initialized
	//void InitSphereIndices();
	//void InitCylinderIndices();
	//void AddCircleIndices(unsigned int* indices, int index, int offset = 0);

	//Extends the Array used when Array size reaches its capacity
	void Extend();

	//assisting function
	float * GetNormals(int shapeType);
	//unsigned int* GetIndexPointer(int index);
	//void AddShape(float* element, int size, int shapeType, float x0, float y0, float z0, float d);//helps with shape creation
	void AddShape(Shape shape);
	
	//Random functions
	//int RandomInt(int min, int max);
	//float RandomFloat(float min, float max);
};