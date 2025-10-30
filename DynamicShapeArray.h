#pragma once
#include "ShapeFactory.h"

#define GLOBAL_SPEED 60
#define MAX_SPEEDUP 100


extern bool soundsEnabled;

class DynamicShapeArray
{
public:
	DynamicShapeArray();
	~DynamicShapeArray();

	void InitFactoryPrototypes();
	//creates Shapes and adds them to the Array
	void CreateRandomShape();
	void CreateRandomShapes(int amount);
	void CreateShape(float x, float y, float z, int size, int ShapeType);
	
	//Binds VAO and ib of the shape at the index
	void BindShape(int index);

	//movement
	void Move(int index);
	void MoveSphere(int index, glm::vec3 speed);
	void SpeedUP(bool up);

	//Getters
	inline int getSize() { return size; };//Returns the size of the ShapeArray
	inline glm::mat4 getModel(int index) { return shapeArray[index].Model; };
	inline glm::mat4 getNormalModel(int index) { return shapeArray[index].normalModel; };
	float * GetColor(int index);//Returns the color of the shape to pass into the shader
	int GetIndexPointerSize(int index);//Returns the size of the ib to use when drawing

	//Setters
	void SetColor(int index, float r_value, float g_value, float b_value, float alpha_value = 1.0f);
	void SetRandomColor(int index, float alpha_value = 1.0f);
	void setRenderer(OpenGLRenderer* renderer);


private:
	//Shape* shapeArray;
	std::vector<Shape> shapeArray;
	ShapeFactory* shapeFactory;
	int size;
	int capacity;
	
	//collision handling
	void CheckCollision(int index);
	void Collide(int index1, int index2);
	
	//Extends the Array used when Array size reaches its capacity, Deprecated
	//void Extend();

	//assisting function
	float * GetNormals(int shapeType);
	void AddShape(Shape shape);
};