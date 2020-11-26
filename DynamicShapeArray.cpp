#include "DynamicShapeArray.h"
#include <cstdlib>
#include <chrono>
#include <random>
#ifdef _WIN32
#include <Windows.h>
#undef max
#endif
/*
	Indices for cube triangle points have been numbered in the following way on the 2 faces back and front(+4)
	1 - 3 5 - 7
	| X | | X |
	0 - 2 4 - 6
	back: 0123, front: 4567, left: 0145, right: 2367, bottom: 4062, top: 5173
	The only easy enough to do by hand
*/

float cube_normals[] = {
		-sqrt(2.0f),-sqrt(2.0f),-sqrt(2.0f),
		-sqrt(2.0f),sqrt(2.0f),-sqrt(2.0f),
		sqrt(2.0f),-sqrt(2.0f),-sqrt(2.0f),
		sqrt(2.0f),sqrt(2.0f),-sqrt(2.0f),
		-sqrt(2.0f),-sqrt(2.0f),sqrt(2.0f),
		-sqrt(2.0f),sqrt(2.0f),sqrt(2.0f),
		sqrt(2.0f),-sqrt(2.0f),sqrt(2.0f),
		sqrt(2.0f),sqrt(2.0f),sqrt(2.0f)
};

float sphere_normals[2109];
float cylinder_normals[216];
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
	int shapeType = RandomInt(0, 2);
	int shape_size = RandomInt(1, 10);
	float r, g, b, vx , vy , vz;
	r = RandomFloat(0.0f, 1.0f);
	g = RandomFloat(0.0f, 1.0f);
	b = RandomFloat(0.0f, 1.0f);
	vx = RandomFloat(0.0f, 0.9f);
	vy = RandomFloat(0.0f, 0.9f);
	vz = RandomFloat(0.0f, 0.9f);
	CreateShape(0.0f, 0.0f, 0.0f, shape_size, shapeType);
	std::cout << "r: " << r << " g: " << g << " b: " << b << ", " << shape_size << " "<< shapeType<< std::endl;
	SetColor(size - 1,r,g,b);
	SetSpeed(size - 1, vx, vy, vz);
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
		CreateSphere(x+ size / 2.0f, y+ size / 2.0f, z+ size / 2.0f, size / 2.0f);
		break;
	case T_CYLINDER:
		CreateCylinder(x+ size / 2.0f, y, z+ size / 2.0f, size / 2.0f, size);
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
- this is needed to draw the right amount of triangles for each shape
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

void DynamicShapeArray::Collide(int index1, int index2) {
	if (shapeArray[index2].speed[0] == 0 && shapeArray[index2].speed[1] == 0 && shapeArray[index2].speed[2] == 0) {
		return;
	}

	int shapeType2 = shapeArray[index2].shapeType;
	int shapeType1 = shapeArray[index1].shapeType;
	float* pos = shapeArray[index1].center;
	float* pos1 = shapeArray[index2].center;
	glm::vec3 speed(shapeArray[index2].speed[0], shapeArray[index2].speed[1], shapeArray[index2].speed[2]);
	glm::vec3 Tspeed(0.0f, 0.0f, 0.0f);
	float dx = pos[0] - pos1[0];
	float dy = pos[1] - pos1[1];
	float dz = pos[2] - pos1[2];

	if (shapeType1 == T_SPHERE) {
		//std::cout << "We have a Collision" << std::endl;
		glm::vec3 centerToCenter(dx, dy, dz);
		centerToCenter = glm::normalize(centerToCenter);
		Tspeed=glm::normalize(speed);
		//std::cout << glm::length(speed) << std::endl;
		speed = glm::length(speed) * normalize((-centerToCenter) * glm::dot(Tspeed, centerToCenter));
		//std::cout<< glm::length(speed) << std::endl;
		shapeArray[index2].speed[0] = speed[0];
		shapeArray[index2].speed[1] = speed[1];
		shapeArray[index2].speed[2] = speed[2];
	}
	if (shapeType1 == T_CUBE) {
		glm::vec3 X(1.0f, 0.0f, 0.0f);
		glm::vec3 Y(0.0f, 1.0f, 0.0f);
		glm::vec3 Z(0.0f, 0.0f, 1.0f);

		glm::vec3 centerToCenter(dx, dy, dz);

		centerToCenter = glm::normalize(centerToCenter);
		float dists[3] = { glm::dot(centerToCenter,X),  glm::dot(centerToCenter,Y),  glm::dot(centerToCenter,Z) };
		float m = abs(dists[0]);
		m = std::max(m, abs(dists[1]));
		m = std::max(m, abs(dists[2]));
		glm::vec3 M;
		//std::cout << "Hello geiassss" << std::endl;
		if (m == abs(dists[0])) {
			shapeArray[index2].speed[0] = -speed[0];
		}if (m == abs(dists[1])) {
			shapeArray[index2].speed[1] = -speed[1];
		}if (m == abs(dists[2])) {
			shapeArray[index2].speed[2] = -speed[2];
		}
	}
	if (shapeType1 == T_CYLINDER) {
		glm::vec3 X(1.0f, 0.0f, 0.0f);
		glm::vec3 Y(0.0f, 1.0f, 0.0f);
		glm::vec3 Z(0.0f, 0.0f, 1.0f);

		glm::vec3 centerToCenter(dx, dy, dz);
		float size1 = shapeArray[index1].size / 2;
		float size2 = shapeArray[index2].size / 2;
		Tspeed = glm::normalize(speed);
		centerToCenter = glm::normalize(centerToCenter);
		glm::vec3 ctc2(centerToCenter[0], 0, centerToCenter[2]);
		float dists[2] = { cos((acos(abs(glm::dot(centerToCenter,X))) + acos(abs(glm::dot(centerToCenter,Z))))/2),  glm::dot(centerToCenter,Y) };
		float m = abs(dists[1]);
		m = std::max(m, abs(dists[0]));
		//m = std::max(m, abs(dists[2]));
		glm::vec3 M;

		if ((dists[1]>=SQRT_2/2&&dists[1]<1)|| (dists[1] <= -SQRT_2 / 2 && dists[1] > -1)) {
			shapeArray[index2].speed[1] = -speed[1];
			//std::cout << "1. AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA" << std::endl;
		}else if (dy < size1) {
			//std::cout << "2. SEXSEXSEXSEXSEXSEXSEXSEX" << std::endl;
			Tspeed = glm::length(speed) * normalize((-centerToCenter) * glm::dot(Tspeed, centerToCenter));
			shapeArray[index2].speed[0] = Tspeed[0];
			shapeArray[index2].speed[2] = Tspeed[2];
		}
	}
}

void DynamicShapeArray::CheckCollision(int index) {
	if (index >= size || index < 2) {
		return;
	}
	bool hasCollision;
	float bigPos[] = { shapeArray[index].center[0] + shapeArray[index].speed[0],
		shapeArray[index].center[1] + shapeArray[index].speed[1],
		shapeArray[index].center[2] + shapeArray[index].speed[2] };
	float nextPos1[3];

	float* pos, *pos1;
	int shapeType = shapeArray[index].shapeType, j, s;
	float dsqr, dx, dy, dz, size0 = shapeArray[index].d, size1;
	for (int i = 0; i < size; i++) {
		hasCollision = false;
		j = index;
		s = i;
		nextPos1[0] = shapeArray[i].center[0] + shapeArray[i].speed[0];
		nextPos1[1] = shapeArray[i].center[1] + shapeArray[i].speed[1];
		nextPos1[2] = shapeArray[i].center[2] + shapeArray[i].speed[2];
		pos = bigPos;
		pos1 = nextPos1;
		if (i != index) {
			if (shapeArray[j].shapeType == T_SPHERE || (shapeArray[s].shapeType == T_CUBE && shapeArray[j].shapeType == T_CYLINDER)) {
				j = i;
				s = index;
				pos1 = bigPos;
				pos = nextPos1;
			}
			shapeType = shapeArray[j].shapeType;
			size0 = shapeArray[s].d;
			size1 = shapeArray[j].d;
			dx = abs(pos[0] - pos1[0]);
			dy = abs(pos[1] - pos1[1]);
			dz = abs(pos[2] - pos1[2]);
			if (shapeArray[s].shapeType == T_SPHERE) {
				if (shapeArray[j].shapeType == T_SPHERE) {
					dsqr = dx * dx + dy * dy + dz * dz;
					//std::cout << "dsqr " << dsqr << " " << (size0 / 2 + size1 / 2) * (size0 / 2 + size1 / 2) << std::endl;
					hasCollision = (dsqr <= (size0 / 2 + size1 / 2) * (size0 / 2 + size1 / 2)) && (dsqr >= size1 * size1 / 4);
					//std::cout << "dsqr: " << sqrt(dsqr) << " what are you? " << (size0+size1) * (size0 + size1)/4 << std::endl;
				}
				else if (shapeArray[j].shapeType == T_CUBE) {

					if (dx >= (size1 / 2 + size0 / 2)) { hasCollision = false; }
					else if (dy >= (size1 / 2 + size0 / 2)) { hasCollision = false; }
					else if (dz >= (size1 / 2 + size0 / 2)) { hasCollision = false; }
					//be completely in
					else if ((dx < abs((size1  - size0) / 2)) && (dy < abs((size1 - size0) / 2)) && (dz < abs((size1 - size0) / 2))) { hasCollision = false; }

					else if (dx < (size1 / 2)) { hasCollision = true; }
					else if (dy < (size1 / 2)) { hasCollision = true; }
					else if (dz < (size1 / 2)) { hasCollision = true; }
					else {
						float cornerDistance_sq = ((dx - size1 / 2) * (dx - size1 / 2)) +
							((dy - size1 / 2) * (dy - size1 / 2)) +
							((dz - size1 / 2) * (dz - size1 / 2));
						hasCollision = (cornerDistance_sq < (size0 / 2 * size0 / 2));
						//std::cout << "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAmakaronia: " << hasCollision << " aaa: " << sqrt(cornerDistance_sq) << " bbb: " << dx << " " << (dx-size1/2) << " " << (dy - size1 / 2) << std::endl;
					}
					
				}
				else if (shapeArray[j].shapeType == T_CYLINDER) {
					dsqr = dx * dx + dz * dz;
					//hasCollision = (dsqr <= (((size0 / 2) + (size1 / 2)) * ((size1/2) + (size0/2))) && dy <= (size1 / 2));
					if (dx > (size1 / 2 + size0 / 2)) { hasCollision = false; }
					else if (dy > (size1 / 2 + size0 / 2)) {  hasCollision = false; }
					else if (dz > (size1 / 2 + size0 / 2)) { hasCollision = false; }
					else if ((dsqr <= (((size0 / 2) + (size1 / 2)) * ((size1 / 2) + (size0 / 2))) && dy <= (size1 / 2))) { hasCollision = true; }
					else {
						dsqr = dx * dx + dy * dy + dz * dz;
						//std::cout << " skata me gala:" << dsqr <<" kai: "<< (size0 / 2 + SQRT_2 * size1 / 2) * (size0 / 2 + SQRT_2 * size1 / 2) << std::endl;
						hasCollision = (dsqr <= (size0 / 2 + SQRT_2 * size1 / 2) * (size0 / 2 + SQRT_2 * size1 / 2));

					}
					if (hasCollision) {
						//std::cout << "makaronia: " << hasCollision << " " << dx << " " << dy << " " << dz << " " << abs((size1 - size0) / 2) << std::endl;
					}
				}
			}
			else if (shapeArray[s].shapeType == T_CUBE) {
				if (shapeArray[j].shapeType == T_CUBE) {
					hasCollision = dx <= size1 / 2 + size0 / 2 && dy <= size1 / 2 + size0 / 2 && dz <= size1 / 2 + size0 / 2 && (dx >= abs(size1 - size0) / 2 || dy >= abs(size1 - size0) / 2 || dz >= abs(size1 - size0) / 2);
					
				}
			}
			else if (shapeArray[s].shapeType == T_CYLINDER) {
				if (shapeArray[j].shapeType == T_CUBE) {
					if (dx >= (size1 / 2 + size0 / 2)) { hasCollision = false; }
					else if (dy >= (size1 / 2 + size0 / 2)) { hasCollision = false; }
					else if (dz >= (size1 / 2 + size0 / 2)) { hasCollision = false; }
					//be completely in 
					else if ((dx < abs((size1 / 2) - (size0 / 2)) && (dy < abs((size1 / 2) - (size0 / 2))) && (dz < abs((size1 / 2) - (size0 / 2))))) { hasCollision = false; }

					else if (dx < (size1 / 2)) { hasCollision = true; }
					else if (dy < (size1 / 2)) { hasCollision = true; }
					else if (dz < (size1 / 2)) { hasCollision = true; }
					else {
						float cornerDistance_sq = ((dx - size1 / 2) * (dx - size1 / 2)) +
							((dy - size1 / 2) * (dy - size1 / 2)) +
							((dz - size1 / 2) * (dz - size1 / 2));
						hasCollision = (cornerDistance_sq < (size0* size0 / 2));
					}
				}
				else if (shapeArray[j].shapeType == T_CYLINDER) {
					dsqr = dx * dx + dz * dz;
					//hasCollision = (dsqr <= (size1 / 2) * (size1 / 2) + (size0 / 2) * (size0 / 2) && dy <= ((size1 / 2) + (size0 / 2)));
					if (dsqr > (size1 / 2 + size0 / 2) * (size1 / 2 + size0 / 2)) { hasCollision = false; }
					else if (dy >= (size1 / 2 + size0 / 2)) { hasCollision = false; }
					else if (dy < (size1 / 2)) { hasCollision = true; }
					else {
						dsqr = dx * dx + dy * dy + dz * dz;
						hasCollision = dsqr <= (SQRT_2 * size0 / 2 + SQRT_2 * size1 / 2) * (SQRT_2 * size0 + SQRT_2 * size1 / 2);
						//std::cout << " makaronia ktyphsa" << std::endl;
					}
				}

			}
			if (hasCollision) {
				//std::cout << "			ponaw " << std::endl;
				Collide(s, j);
				Collide(j, s);
				#ifdef _WIN32
				if(size<=10){
					PlaySound(TEXT("collision.wav"), NULL, SND_FILENAME | SND_ASYNC);
				}
				#endif
			}
		}
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

void DynamicShapeArray::MoveSphere(int index, glm::vec3 speed)
{
	int next_center[3] = { shapeArray[index].center[0] + speed[0],
	shapeArray[index].center[1] + speed[1],
	shapeArray[index].center[2] + speed[2]};
	float upper_limit = 100.0f - shapeArray[index].d / 2;
	float lower_limit = 0 + shapeArray[index].d / 2;
	CheckCollision(index);
	if (next_center[0] > upper_limit || next_center[1] > upper_limit || next_center[2] > upper_limit || next_center[0] < lower_limit || next_center[1] < lower_limit || next_center[2] < lower_limit)
		return;
	shapeArray[index].Model = glm::translate(glm::mat4(1.0f), speed) * shapeArray[index].Model;
	shapeArray[index].center[0] = next_center[0];
	shapeArray[index].center[1] = next_center[1];
	shapeArray[index].center[2] = next_center[2];
	//std::cout << shapeArray[index].center[0] << ", " << shapeArray[index].center[1] << ", " << shapeArray[index].center[2] << "," << std::endl;
}


//Private functions

void DynamicShapeArray::AddArray(float * element, int elementSize, int shapeType, float x0, float y0, float z0, float d) {
	if (capacity == size) {
		Extend();
	}

	float * tmpData = (float*)malloc(elementSize * sizeof(float));
	if (tmpData != nullptr) {
		for (int i = 0; i < elementSize; i++) {
			tmpData[i] = element[i];
		}
		int index = size++;
		shapeArray[index].data = tmpData;
		shapeArray[index].size = elementSize;
		shapeArray[index].shapeType = shapeType;
		shapeArray[index].Model = glm::mat4(1.0f);
		shapeArray[index].speed[0] = 0.0f;
		shapeArray[index].speed[1] = 0.0f;
		shapeArray[index].speed[2] = 0.0f;
		shapeArray[index].center[0] = x0;
		shapeArray[index].center[1] = y0;
		shapeArray[index].center[2] = z0;
		shapeArray[index].d = d;
		if (size > 2){
			shapeArray[index].collisions = (bool*)malloc((size) * sizeof(bool));
		}
		else {
			shapeArray[index].collisions = nullptr;
		}
		for (int i = 2; i < (size-1); i++) {
			shapeArray[i].collisions = (bool*)realloc(shapeArray[i].collisions,(size) * sizeof(bool));
		}
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
	AddArray(positions, 8 * 3, T_CUBE, x0+size/2, y0 + size / 2, z0 + size / 2, size);
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

	if (firstCylinder) {
		cylinder_normals[0] = cylinder_normals[2] = cylinder_normals[108] = cylinder_normals[110] = 0;
		cylinder_normals[1] = -34 * cos((2 * PI) / 34);
		cylinder_normals[109] = 34 * cos((2 * PI) / 34);
		for (int i = 1, n = 1; i < 36; i++) {
			//std::cout << "normal: " << N.x << " , " << N.y << " , " << N.z << std::endl;
			cylinder_normals[3 * i] = cos(2 * PI * n / 34) + cos(2 * PI * (n + 1) / 34) + cos(PI * (2 * n + 1) / 34);
			cylinder_normals[3 * i + 1] = -2 * sin((2 * PI) / 34);
			cylinder_normals[3 * i + 2] = sin(2 * PI * n / 34) + sin(2 * PI * (n + 1) / 34) + sin(PI * (2 * n + 1) / 34);
			cylinder_normals[3 * i + 108] = cos(2 * PI * n / 34) + cos(2 * PI * (n + 1) / 34) + cos(PI * (2 * n + 1) / 34);
			cylinder_normals[3 * i + 109] = 2 * cos((2 * PI) / 34);
			cylinder_normals[3 * i + 110] = sin(2 * PI * n / 34) + sin(2 * PI * (n + 1) / 34) + sin(PI * (2 * n + 1) / 34);
			n += 1;
		}
		firstCylinder = false;
	}

	AddArray(cylinder_pos, 216, T_CYLINDER, x, y + height / 2, z, 2 * radius);

}

void DynamicShapeArray::Move(int index) {
	float * speed = shapeArray[index].speed;
	float size = shapeArray[index].size;
	if (speed[0] || speed[1] || speed[2]) {
		CheckCollision(index);
		shapeArray[index].Model = glm::translate(glm::mat4(1.0f), glm::vec3(speed[0], speed[1], speed[2])) * shapeArray[index].Model;
		shapeArray[index].center[0] += speed[0];
		shapeArray[index].center[1] += speed[1];
		shapeArray[index].center[2] += speed[2];
	}
}

glm::mat4 DynamicShapeArray::GetModel(int index) {
	return shapeArray[index].Model;
}

float* DynamicShapeArray::CreateCircle(float x, float y, float z, float radius) {
	int num_of_sides = CIRCLE_TRIANGLE_NUM;
	int num_of_vertices = num_of_sides + 2;
	int n = 3 * num_of_vertices;
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
	std::cout << x0 << ", " << y0 << ", " << z0 << "," << std::endl;
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

			n += 3;
		}
	} 
	AddArray(points, size, T_SPHERE, x0, y0, z0, 2*radius);
}

float* DynamicShapeArray::GetNormals(int shapeType) {
	switch (shapeType)
	{
	case T_CUBE:
		return cube_normals;
	case T_SPHERE:
		return sphere_normals;
	case T_CYLINDER:
		return cylinder_normals;
	}
	return nullptr;
}

void DynamicShapeArray::createBuffer(int index) {
	unsigned int buffer_id;
	float * shape = shapeArray[index].data;
	int shape_size = shapeArray[index].size;
	int index_pointer_size = GetIndexPointerSize(index);
	int index_pointer_size2 = GetIndexPointerSize(index);
	float* normals = GetNormals(shapeArray[index].shapeType);
	if (shapeArray[index].shapeType == T_CUBE) {
		index_pointer_size2 = 24;
	}
	else if (shapeArray[index].shapeType == T_SPHERE) {
		index_pointer_size2 = 2109;
	}
	else if (shapeArray[index].shapeType == T_CYLINDER) {
		index_pointer_size2 = 216;
	}
	unsigned int * index_array = GetIndexPointer(index);

	unsigned int vao;
	//create and bind the vao
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	
	//create a buffer to keep out positions
	glGenBuffers(1, &buffer_id);
	glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
	glBufferData(GL_ARRAY_BUFFER, shape_size * sizeof(float) + index_pointer_size2 * sizeof(float), 0, GL_STATIC_DRAW);
	
	glBufferSubData(GL_ARRAY_BUFFER, 0, shape_size * sizeof(float), shape);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
	
	glBufferSubData(GL_ARRAY_BUFFER, shape_size * sizeof(float), index_pointer_size2 * sizeof(float), normals);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)(shape_size * sizeof(float)));

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
			}

			if (i != (SPHERE_STACK_NUM - 1))
			{
				sphere_indices[n++] = k1 + 1;
				sphere_indices[n++] = k2;
				sphere_indices[n++] = k2 + 1;
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