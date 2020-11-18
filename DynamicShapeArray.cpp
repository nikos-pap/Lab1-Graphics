#include "DynamicShapeArray.h"
//#define _USE_MATH_DEFINES
//#include <math.h>
#include <cstdlib>


/*
	Indices for cube triangle points have been numbered in the following way on the 2 faces back and front(+4)
	1 - 3 5 - 7
	| X | | X |
	0 - 2 4 - 6
	back: 0123, front: 4567, left: 0145, right: 2367, bottom: 4062, top: 5173
	to get actual pos though you need to multiply with 3
*/
unsigned int cube_indices[] = {
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

unsigned int circle_indices[] = {
		0, 1, 2,
		3, 1, 4,
		5, 1, 6,
		7, 1, 8,
		9, 1, 10,
		11, 1, 12,
		13, 1, 14,
		15, 1, 16,
		17, 1, 18,
		19, 1, 20,
		21, 1, 22,
		23, 1, 24,
		25, 1, 26,
		27, 1, 28,
		29, 1, 30,
		31, 1, 32,
		33, 1, 34,
		35, 1, 36,
		37, 1, 38,
		39, 1, 40,
		41, 1, 42,
		43, 1, 44,
		45, 1, 46,
		47, 1, 48,
		49, 1, 50,
		51, 1, 52,
		53, 1, 54,
		55, 1, 56,
		57, 1, 58,
		59, 1, 60,
		61, 1, 62,
		63, 1, 64,
		65, 1, 66,
		67, 1, 68,
		69, 1, 70,
		71, 1, 72,
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
		int index = size++;
		shapeArray[index] = { tmpData,elementSize, shapeType };
		createBuffer(index);
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

unsigned int* DynamicShapeArray::GetIndexPointer(int shapeType)
{
	switch (shapeType)
	{
	case T_CUBE:
		return &cube_indices[0];
	case T_CIRCLE:
		return &circle_indices[0];
	}
	return nullptr;
}

int DynamicShapeArray::GetIndexPointerSize(int shapeType)
{
	switch (shapeType)
	{
	case T_CUBE:
		return 36;
	case T_CIRCLE:
		return 108;
	}
	return 0;
}

void DynamicShapeArray::printData(int id) {
	for (int shape = 0; shape < 24; shape++) {
		std::cout << shapeArray[id].data[shape] << std::endl;
	}
}

glm::vec3 * DynamicShapeArray::GetCubeNormals(int id, int *n)
{
	*n = 12;
	glm::vec3 * normals = (glm::vec3 *) malloc(12*3*sizeof(float));
	if (normals != nullptr) {
		for (int shape = 0,n = 0; shape < 36; n++) {
			unsigned int p1_i = 3 * cube_indices[shape++];
			unsigned int p2_i = 3 * cube_indices[shape++];
			unsigned int p3_i = 3 * cube_indices[shape++];
			glm::vec3 p1 = glm::vec3(shapeArray[id].data[p1_i], shapeArray[id].data[p1_i + 1], shapeArray[id].data[p1_i + 2]);
			glm::vec3 p2 = glm::vec3(shapeArray[id].data[p2_i], shapeArray[id].data[p2_i + 1], shapeArray[id].data[p2_i + 2]);
			glm::vec3 p3 = glm::vec3(shapeArray[id].data[p3_i], shapeArray[id].data[p3_i + 1], shapeArray[id].data[p3_i + 2]);
			glm::vec3 u, v, N;
			u = p2 - p1;
			v = p3 - p1;
			N = normalize(cross(u, v));
			normals[n] = N;
		}
	}
	return normals;
}

unsigned int DynamicShapeArray::GetVAOID(int index)
{
	return shapeArray[index].vao_id;
}

unsigned int DynamicShapeArray::GetIBOID(int index)
{
	return shapeArray[index].ib_id;
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

void DynamicShapeArray::SetColor(int index, float r_value, float g_value, float b_value, float alpha_value)
{
	if (index < size) {
		shapeArray[index].color[0] = r_value;
		shapeArray[index].color[1] = g_value;
		shapeArray[index].color[2] = b_value;
		shapeArray[index].color[3] = alpha_value;
	}
}

void DynamicShapeArray::SetSpeed(int index, float ux, float uy, float uz)
{
	if (index < size) {
		shapeArray[index].speed[0] = ux; 
		shapeArray[index].speed[1] = uy; 
		shapeArray[index].speed[2] = uz;
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

void DynamicShapeArray::CreateCircle(float x, float y, float z, float radius) {
	int num_of_sides = 34;
	int num_of_vertices = num_of_sides + 2;
	int n = 3 + (2 * num_of_vertices);
	float twicePi = 2.0f * 3.14159256f;
	float * vertices = (float *) malloc(sizeof(float) * n);
	if (vertices != nullptr) {
		vertices[0] = x;
		vertices[1] = y;
		vertices[2] = z;
		for (int i = 3; i < num_of_vertices;)
		{
			vertices[i++] = x + (radius * cos(i * twicePi / num_of_sides));
			vertices[i++] = z + (radius * sin(i * twicePi / num_of_sides));
		}
		AddArray(vertices, (2 * (num_of_vertices)+3), T_CIRCLE);
	}
}

void DynamicShapeArray::DrawShape(int index, Shader shader)
{
	Shape shape = shapeArray[index];
	unsigned int vao = shape.vao_id;
	unsigned int ib = shape.ib_id;
	unsigned int ib_size = GetIndexPointerSize(index);
	//bind everything
	//shader.Bind();
	std::cout << vao << ":" << ib << std::endl;
	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);
	glDrawElements(GL_TRIANGLES, ib_size, GL_UNSIGNED_INT, nullptr);
}

void DynamicShapeArray::Bindshape(int index) {
	Shape shape = shapeArray[index];
	unsigned int vao = shape.vao_id;
	unsigned int ib = shape.ib_id;
	unsigned int ib_size = GetIndexPointerSize(index);
	//bind everything
	//shader.Bind();
	std::cout << vao << ":" << ib << std::endl;
	glBindVertexArray(vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);
}

void DynamicShapeArray::createBuffer(int shape_index) {
	unsigned int buffer_id;
	float * shape = GetShape(shape_index);
	int shape_size = GetSize(shape_index);
	int index_pointer_size = GetIndexPointerSize(shape_index);
	unsigned int * index_array = GetIndexPointer(shape_index);

	unsigned int vao;
	//create and bind the vao
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	//create a buffer to keep out positions
	glGenBuffers(1, &buffer_id);
	glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
	glBufferData(GL_ARRAY_BUFFER, shape_size * sizeof(float), shape, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);

	//create a buffer for the indexes
	unsigned int ibo;
	//glGenBuffers creates the random id for that buffer and stores it in the variable
	glGenBuffers(1, &ibo);
	//bind object buffer to target
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,  index_pointer_size * sizeof(unsigned int), index_array, GL_STATIC_DRAW);

	//keep the three buffers in the shape
	shapeArray[shape_index].vao_id = vao;
	shapeArray[shape_index].vb_id = buffer_id;
	shapeArray[shape_index].ib_id = ibo;
	std::cout << "buffer created id's are:" << vao << ", " << buffer_id << ", " << ibo << std::endl;
}