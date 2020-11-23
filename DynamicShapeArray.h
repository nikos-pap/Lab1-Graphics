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

#define SPHERE_SECTOR_NUM 36
#define SPHERE_STACK_NUM 18
#define CIRCLE_TRIANGLE_NUM 34

#define GLOBAL_SPEED 60
//this is for a new implementation I thought subject to change
/*
struct Original {
	unsigned int* shape_indices;
	unsigned int* vertex_array;
	
	unsigned int vao_id;
	unsigned int ib_id;
	unsigned int vb_id;
	
	int index_size;
	int vertex_size;
};*/

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
	bool* collisions;
	float center[3];
	float d;
/*public:
	bool operator>(const Shape other) {
		return center > other.center;
	}

	bool operator<(const Shape other) {
		return center < other.center;
	}*/
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

	void CheckCollision(int index);
	//Getters
	//Returns the color of the shape to pass into the shader
	float * GetColor(int index);
	
	//Returns the size of the ShapeArray
	inline int GetSize() { return size; };
	glm::mat4 GetModel(int index);
	//Returns the size of the ib to use when drawing
	int GetIndexPointerSize(int shapeType);

	//Returns the Normals of the Cube might replace it with a function that works for every shape
	void GetCubeNormals();//needs a fix for sure we should not always calculate this
	void Move(int index);
	float * GetSpeed(int index); //not used fix, delete or use it
	float * GetShape(int index); //not used in the new implementation consider removing
	int GetSize(int index); //not used in the new implementation consider removing
	unsigned int* GetIndexPointer(int shapeType);//not used in the new implementation consider removing
	unsigned int GetVAOID(int index); //not used in the new implementation consider removing
	unsigned int GetIBOID(int index); //not used in the new implementation consider removing
	
	//Setters
	void SetColor(int id, float r_value, float g_value, float b_value, float alpha_value = 1.0f);
	
	void SetSpeed(int id, float ux, float uy, float uz);//not used fix, delete or use it
	void SetVAOID(int index, int id); //not used in the new implementation consider removing
	void SetBufferID(int index, int id);//not used in the new implementation consider removing
	void SetIBOID(int index, int id); //not used in the new implementation consider removing

private:
	Shape* shapeArray;
	int size = 0;
	int capacity;

	//newly private functions check if all OK
	int RandomInt(int min, int max);
	float RandomFloat(float min, float max);
	void AddArray(float* element, int size, int shapeType, float x0, float y0, float z0, float d);
	void CreateCube(float x0, float y0, float z0, float size);
	void CreateCylinder(float x, float y, float z, float radius, float height);
	float* CreateCircle(float x, float y, float z, float radius);
	void CreateSphere(float x0, float y0, float z0, float radius);
	void createBuffer(int index);
	//-----------------------------------------

	//functions that initialize index arrays they run when shapeArray is initialized
	void InitCylinderIndices();
	void AddCircleIndices(unsigned int* indices, int index, int offset = 0);
	void InitSphereIndices();

	//Extends the Array used when Array size reaches its capacity
	void Extend();

	int GetShapeType(int index);//not used fix or consider removing
	void printData(int index);//not used fix or consider removing
};