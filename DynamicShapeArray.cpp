#include "DynamicShapeArray.h"
#include <cstdlib>
#include <chrono>
#include <random>

/*
	Indices for cube triangle points have been numbered in the following way on the 2 faces back and front(+4)
	1 - 3 5 - 7
	| X | | X |
	0 - 2 4 - 6
	back: 0123, front: 4567, left: 0145, right: 2367, bottom: 4062, top: 5173
	The only easy enough to do by hand
*/

float cube_normals[36];
float sphere_normals[2 * 3 * (SPHERE_STACK_NUM - 1) * SPHERE_SECTOR_NUM];
float cylinder_normals[CIRCLE_TRIANGLE_NUM * 3 * 4];
bool firstCylinder = true;

unsigned int cube_indices[] = {
		4, 6, 5,//front
		7, 5, 6,//front
		0, 1, 2,//back
		3, 2, 1,//back
		0, 4, 1,//left
		5, 1, 4,//left
		2, 3, 6,//right
		3, 7, 6,//right
		5, 7, 1,//top
		3, 1, 7,//top
		4, 0, 6,//bottom
		2, 6, 0//bottom
};

unsigned int cylinder_indices[CIRCLE_TRIANGLE_NUM * 3 * 4];
unsigned int * sphere_indices;

/*
Simple Constructor
-initializes the Array with a capacity of 10
*/
DynamicShapeArray::DynamicShapeArray() {
	capacity = 10;
	shapeArray = (Shape *) malloc(capacity * sizeof(Shape));
	size = 0;
    InitSphereIndices();
	InitCylinderIndices();
	GetCubeNormals();
}

/*
Simple Destructor
-freeing the dynamically allocated space
*/
//remember to free the rest
DynamicShapeArray::~DynamicShapeArray() {
	free(shapeArray);
	//free(cylinder_indices);
	//free(sphere_indices);
}


int DynamicShapeArray::RandomInt(int min, int max) {
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine generator(seed);
	std::uniform_int_distribution<int> distributionInteger(min, max);
	return distributionInteger(generator);
}

float DynamicShapeArray::RandomFloat(float min, float max) {
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine generator(seed);
	std::uniform_real_distribution<float> distributionDouble(min, max);
	return (float) distributionDouble(generator);
}

void DynamicShapeArray::CreateRandomShape() {
	int shapeType = RandomInt(0, 3);
	int shape_size = RandomInt(1, 10);
	float r, g, b;
	r = RandomFloat(0.0f, 1.0f);
	g = RandomFloat(0.0f, 1.0f);
	b = RandomFloat(0.0f, 1.0f);
	std::cout << "r: " << r << " g: " << g << " b: " << b << std::endl;
	CreateShape(0.0f, 0.0f, 0.0f, shape_size, shapeType);
	SetColor(size - 1,r,g,b);
}

/*
Shape Creator
-creates new shape to add to the Array
*/
void DynamicShapeArray::CreateShape(float x, float y, float z, int size, int ShapeType) {
	switch (ShapeType)
	{
	case T_CUBE:
		CreateCube(x, y, z, size);
		break;
	case T_SPHERE:
		CreateSphere(x+ size / 2, y+ size / 2, z+ size / 2, size / 2);
		break;
	case T_CYLINDER:
		CreateCylinder(x+ size / 2, y, z+ size / 2, size / 2, size);
		break;
	}
}

/*
Buffer Binder
- binds shape's vao and ibo
- created mainly because it removes 2-3 Getters/Setters
*/
void DynamicShapeArray::BindShape(int index) {
	glBindVertexArray(shapeArray[index].vao_id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shapeArray[index].ib_id);
}


//Getters

/*
Shape Color
-return shape color to pass to shader
*/
float* DynamicShapeArray::GetColor(int id) {
	if (id < size) {
		return shapeArray[id].color;
	}
	return nullptr;
}

/*
Index Buffer Pointer Size
- this is needed to draw the right amound of triangles for each shape
*/
int DynamicShapeArray::GetIndexPointerSize(int index) {
	int shapeType = shapeArray[index].shapeType;
	switch (shapeType)
	{
	case T_CUBE:
		return 36;
	case T_CYLINDER:
		return CIRCLE_TRIANGLE_NUM * 3 * 4;
	case T_SPHERE:
		return 2 * 3 * (SPHERE_STACK_NUM - 1) * SPHERE_SECTOR_NUM;
	}
	return 0;
}

void DynamicShapeArray::GetCubeNormals()
{	
	float x0 = 0, y0 = 0, z0 = 0;
	float x1 = 1, y1 = 1, z1 = 1;

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
	for (int shape = 0,n = 0; shape < 36; n++) {
		int p1_i = 3 * cube_indices[shape++];
		int p2_i = 3 * cube_indices[shape++];
		int p3_i = 3 * cube_indices[shape++];
		glm::vec3 p1 = glm::vec3(positions[p1_i], positions[p1_i + 1], positions[p1_i + 2]);
		glm::vec3 p2 = glm::vec3(positions[p2_i], positions[p2_i + 1], positions[p2_i + 2]);
		glm::vec3 p3 = glm::vec3(positions[p3_i], positions[p3_i + 1], positions[p3_i + 2]);
		glm::vec3 u, v, N;
		u = p2 - p1;
		v = p3 - p1;
		N.x = (u.y * v.z) - (u.z * v.y);
		N.y = (u.z * v.x) - (u.x * v.z);
		N.z = (u.x * v.y) - (u.y * v.x);
		N = normalize(N);
		std::cout << "normal: " << N.x << " , " << N.y << " , " << N.z << std::endl;
		cube_normals[n++] = N.x;
		cube_normals[n++] = N.y;
		cube_normals[n++] = N.z;
	}
}


float * DynamicShapeArray::GetSpeed(int index) {
	if (index < size) {
		return shapeArray[index].speed;
	}

	return nullptr;
}

//Getters that are not needed anymore
float * DynamicShapeArray::GetShape(int index) {
	if (index < size) {
			return shapeArray[index].data;
	}
	
	return nullptr;
}

int DynamicShapeArray::GetSize(int index) {
	if (index < size) {
		return shapeArray[index].size;
	}

	return -1;
}

unsigned int* DynamicShapeArray::GetIndexPointer(int index) {
	int shapeType = shapeArray[index].shapeType;
    switch (shapeType)
    {
    case T_CUBE:
        return &cube_indices[0];
    case T_CYLINDER:
        return &cylinder_indices[0];
    case T_SPHERE:
        return &sphere_indices[0];
    }
	return nullptr;
}

unsigned int DynamicShapeArray::GetVAOID(int index) {
	if (index < size) {
		return shapeArray[index].vao_id;
	}

	return 0;
}

unsigned int DynamicShapeArray::GetIBOID(int index) {
	if (index < size) {
		return shapeArray[index].ib_id;
	}

	return 0;
}

//Setters

/*
Simple Color Setter
- just sets rgba color of shape at index
*/
void DynamicShapeArray::SetColor(int index, float r_value, float g_value, float b_value, float alpha_value) {
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

//Setters that are not used anymore

void DynamicShapeArray::SetBufferID(int index, int id) { 
	shapeArray[index].vb_id = id;
}

void DynamicShapeArray::SetVAOID(int index, int id) {
	shapeArray[index].vao_id = id; 
}

void DynamicShapeArray::SetIBOID(int index, int id) {
	shapeArray[index].ib_id = id; 
}


//Private functions

void DynamicShapeArray::AddArray(float * element, int elementSize, int shapeType) {
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

//Cylinder functions
void DynamicShapeArray::CreateCylinder(float x, float y, float z, float radius, float height) {
	float* circle1, * circle2;
	circle1 = CreateCircle(x, y, z, radius);
	circle2 = CreateCircle(x, (y + height), z, radius);
	float cylinder_pos[216];
	for (int i = 0; i < 108; i++) {
		cylinder_pos[i] = circle1[i];
		cylinder_pos[i + 108] = circle2[i];
	}
	AddArray(cylinder_pos, 216, T_CYLINDER);

	if (firstCylinder) {
		for (int shape = 0, n = 0; shape < CIRCLE_TRIANGLE_NUM * 3 * 4; n++) {
			int p1_i = 3 * cylinder_indices[shape++];
			int p2_i = 3 * cylinder_indices[shape++];
			int p3_i = 3 * cylinder_indices[shape++];
			glm::vec3 p1 = glm::vec3(cylinder_pos[p1_i], cylinder_pos[p1_i + 1], cylinder_pos[p1_i + 2]);
			glm::vec3 p2 = glm::vec3(cylinder_pos[p2_i], cylinder_pos[p2_i + 1], cylinder_pos[p2_i + 2]);
			glm::vec3 p3 = glm::vec3(cylinder_pos[p3_i], cylinder_pos[p3_i + 1], cylinder_pos[p3_i + 2]);
			glm::vec3 u, v, N;
			u = p2 - p1;
			v = p3 - p1;
			N = normalize(cross(u, v));
			//std::cout << "normal: " << N.x << " , " << N.y << " , " << N.z << std::endl;
			cylinder_normals[n++] = N.x;
			cylinder_normals[n++] = N.y;
			cylinder_normals[n++] = N.z;
		}
		firstCylinder = false;
	}

}

float* DynamicShapeArray::CreateCircle(float x, float y, float z, float radius) {
	int num_of_sides = CIRCLE_TRIANGLE_NUM;
	int num_of_vertices = num_of_sides + 2;
	int n = 3 * num_of_vertices;//3 + (2 * num_of_vertices);
	float twicePi = 2.0f * PI;
	float * vertices = (float *) malloc(sizeof(float) * n);
	if (vertices != nullptr) {
		vertices[0] = x;
		vertices[1] = y;
		vertices[2] = z;

		for (int i = 3,p=1; p < num_of_vertices;p++)
		{		
			float tempx, tempz;
			tempx = x + (radius * cos(p * twicePi / num_of_sides));
			tempz = z + (radius * sin(p * twicePi / num_of_sides));
			vertices[i] = tempx;
			i++;
			vertices[i++] = y;
			vertices[i] = tempz;
			i++;
		}

		return vertices;
	}
	return nullptr;
}

//Sphere function
void DynamicShapeArray::CreateSphere(float x0, float y0, float z0, float radius) {

	float x, y, z, xy;                              // vertex position
	float nx, ny, nz, lengthInv = 1.0f / radius;    // vertex normal
	//float s, t;                                    // vertex texCoord
	int size = (SPHERE_SECTOR_NUM + 1) * (SPHERE_STACK_NUM + 1) * 3;
	float sectorStep = 2 * PI / SPHERE_SECTOR_NUM;
	float stackStep = PI / SPHERE_STACK_NUM;
	float sectorAngle, stackAngle;
	float* points;

	points = (float*)malloc(size * sizeof(float));
    if (points == nullptr) {
        std::cout << "Failed to allocate memory for points" << std::endl;
        return;
    }
	for (int i = 0,n=0; i <= SPHERE_STACK_NUM; ++i)
	{
		stackAngle = PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
		xy = radius * cosf(stackAngle);             // r * cos(u)
		z = radius * sinf(stackAngle);              // r * sin(u)

		// add (SPHERE_SECTOR_NUM+1) vertices per stack
		// the first and last vertices have same position and normal, but different tex coords
		for (int j = 0; j <= SPHERE_SECTOR_NUM; ++j)
		{
			sectorAngle = j * sectorStep;           // starting from 0 to 2pi



			// vertex position (x, y, z)
			x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
			y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
			points[n] = x + x0;
			points[n+1] = y + y0;
			points[n+2] = z + z0;

			// calculating normals
			nx = x * lengthInv;
			ny = y * lengthInv;
			nz = z * lengthInv;
			
			sphere_normals[n] = nx;
			sphere_normals[n+1] = ny;
			sphere_normals[n+2] = nz;

			//std::cout << x << ", " << y << ", " << z << "," << std::endl;
			n += 3;
		}
	} 
	AddArray(points, size, T_SPHERE);
}

void DynamicShapeArray::createBuffer(int index) {
	unsigned int buffer_id;
	float * shape = shapeArray[index].data;
	int shape_size = shapeArray[index].size;
	int index_pointer_size = GetIndexPointerSize(index);
	unsigned int * index_array = GetIndexPointer(index);

	unsigned int vao;
	//create and bind the vao
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	//create a buffer to keep out positions
	glGenBuffers(1, &buffer_id);
	glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
	glBufferData(GL_ARRAY_BUFFER, shape_size * sizeof(float) + index_pointer_size * sizeof(float), 0, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, shape_size * sizeof(float), shape);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
	glBufferSubData(GL_ARRAY_BUFFER, shape_size * sizeof(float), index_pointer_size * sizeof(float), cube_normals);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)(shape_size * sizeof(float)));
	//if (shapeArray[index].shapeType == T_CUBE) {
	//	glBufferData(GL_ARRAY_BUFFER, 36 * sizeof(float), cube_normals, GL_STATIC_DRAW);
	//	glEnableVertexAttribArray(1);
	//	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3,(void *) (shape_size * sizeof(float))/*(void *) (36 * sizeof(float))*/);
	//}

	//create a buffer for the indexes
	unsigned int ibo;
	//glGenBuffers creates the random id for that buffer and stores it in the variable
	glGenBuffers(1, &ibo);
	//bind object buffer to target
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,  index_pointer_size * sizeof(unsigned int), index_array, GL_STATIC_DRAW);

	//keep the three buffers in the shape
	shapeArray[index].vao_id = vao;
	shapeArray[index].vb_id = buffer_id;
	shapeArray[index].ib_id = ibo;
	std::cout << "buffer created id's are:" << vao << ", " << buffer_id << ", " << ibo << std::endl;
}

//Initialize Index Arrays
//Cylinder
void DynamicShapeArray::InitCylinderIndices() {
	int offset = CIRCLE_TRIANGLE_NUM + 2;
	//cylinder_indices = (unsigned int*)malloc(CIRCLE_TRIANGLE_NUM * 3 * 4 *sizeof(unsigned int));

	if (cylinder_indices == nullptr) {
		std::cout << "Failed to allocate memory for cylinder_indices" << std::endl;
		exit(-1);
	}
	AddCircleIndices(cylinder_indices, 0);
	AddCircleIndices(cylinder_indices, CIRCLE_TRIANGLE_NUM * 3, offset);

	for (int i = 1, pos = CIRCLE_TRIANGLE_NUM * 3 *2; i <= CIRCLE_TRIANGLE_NUM; i++) {
		cylinder_indices[pos++] = i;
		cylinder_indices[pos++] = offset + i + 1;
		cylinder_indices[pos++] = i + 1;
		cylinder_indices[pos++] = offset + i;
		cylinder_indices[pos++] = i;
		cylinder_indices[pos++] = offset + i + 1;
	}
}

void DynamicShapeArray::AddCircleIndices(unsigned int * indices, int index, int offset) {
	for (int i = 1; i <= CIRCLE_TRIANGLE_NUM; i++) {
		indices[index++] = offset + i;
		indices[index++] = offset;
		indices[index++] = offset + i + 1;
	}
}

//Sphere
void DynamicShapeArray::InitSphereIndices() {
	unsigned int k1, k2;
    sphere_indices = (unsigned int *)malloc(2 * 3 * (SPHERE_STACK_NUM-1) * SPHERE_SECTOR_NUM *sizeof(unsigned int));
	if (sphere_indices == nullptr) {
		std::cout << "Failed to allocate memory for sphere_indices" << std::endl;
		exit(-1);
	}
	for (int i = 0, n = 0; i < SPHERE_STACK_NUM; ++i)
	{
		k1 = i * (SPHERE_SECTOR_NUM + 1);     // beginning of current stack
		k2 = k1 + SPHERE_SECTOR_NUM + 1;      // beginning of next stack

		for (int j = 0; j < SPHERE_SECTOR_NUM; ++j, ++k1, ++k2)
		{
			// 2 triangles per sector excluding 1st and last stacks
			if (i != 0)
			{
				sphere_indices[n++] = k1;
				sphere_indices[n++] = k2;
				sphere_indices[n++] = k1 + 1;
				//addIndices(k1, k2, k1 + 1);   // k1---k2---k1+1
				//std::cout << k1 << ", " << k2 << ", " << k1 + 1 << std::endl;
			}

			if (i != (SPHERE_STACK_NUM - 1))
			{
				sphere_indices[n++] = k1 + 1;
				sphere_indices[n++] = k2;
				sphere_indices[n++] = k2 + 1;
				//std::cout << k1 + 1 << ", " << k2 << ", " << k2 + 1 << std::endl;
				//addIndices(k1 + 1, k2, k2 + 1); // k1+1---k2---k2+1
			}
		}
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

//not needed consider removing
int DynamicShapeArray::GetShapeType(int index) {
	return shapeArray[index].shapeType; 
}

//not needed consider removing
void DynamicShapeArray::printData(int id) {
	for (int shape = 0; shape < 24; shape++) {
		std::cout << shapeArray[id].data[shape] << std::endl;
	}
}