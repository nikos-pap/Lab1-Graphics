#pragma once
#define T_NULL -1
#define T_CUBE 0
#define T_SPHERE 1
#define T_CYLINDER 2
#include <glm/glm.hpp>
#include <array>
struct Shape {
	float * data;
	int size;
	int shapeType;
	float speed[3] = { 0.0f ,0.0f ,0.0f };
	float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
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
	void GetNormals(int id);
	float * GetShape(int index);
	float * GetSpeed(int id);
	float * GetColor(int id);
	unsigned int GetSize(int index);
	
	void SetSpeed(int id, float ux, float uy, float uz);
	void SetColor(int id, float r_value, float g_value, float b_value, float alpha_value = 1.0f);

	inline unsigned int GetSize() const { return size; };
private:
	void printData(int id);
	void Extend();
};