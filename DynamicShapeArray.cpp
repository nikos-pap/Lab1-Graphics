#include "DynamicShapeArray.h"
#include <cstdlib>
#include <chrono>
#include <random>
#ifdef _WIN32
#include <Windows.h>
#undef max
#endif


//Normals
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
float ring_normals[8*(CIRCLE_VERTEX_NUM - 1) * 3];

//indice arrays
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
unsigned int sphere_indices[2 * 3 * (SPHERE_STACK_NUM - 1) * SPHERE_SECTOR_NUM];
unsigned int  ring_indices[2 * 8 * CIRCLE_TRIANGLE_NUM * 3];


float globalSpeed = 0.5f / GLOBAL_SPEED;
int speedUP = 50;

//flags
bool firstCylinder = true;
bool firstRing = true;
bool firstSphere = true;
bool soundsEnabled = true;

/*
*Simple Constructor
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
*Simple Destructor
-freeing the dynamically allocated space
*/
//remember to free the rest
DynamicShapeArray::~DynamicShapeArray() {
	for (int i = 0; i < size; i++) {
		free(shapeArray[i].data);
	}
	free(shapeArray);
}

/*
*Random Shape creator
-generates random shapes and adds them to the ShapeArray
*/
void DynamicShapeArray::CreateRandomShape() {
	int shapeType = RandomInt(0, 3);
	int shape_size = RandomInt(1, 10);
	float r, g, b, vx , vy , vz;
	r = RandomFloat(0.0f, 1.0f);
	g = RandomFloat(0.0f, 1.0f);
	b = RandomFloat(0.0f, 1.0f);
	vx = RandomFloat(0.0f, 0.9f);
	vy = RandomFloat(0.0f, 0.9f);
	vz = RandomFloat(0.0f, 0.9f);
	if (shapeType == T_RING) {
		float r1 = 0.5* shape_size;
		float r2 = r1/(float)RandomInt(3, 10);
		CreateRing(r+5,2*r2+5, r1+5,r1,r2);
	}
	else {
		CreateShape(5.0f, 5.0f, 5.0f, shape_size, shapeType);
	}
	std::cout << "Spawned new shape: " << shapeType << std::endl;
	std::cout << "r: " << r << " g: " << g << " b: " << b << ", size: " << shape_size << std::endl;
	SetColor(size - 1,r,g,b);
	shapeArray[size - 1].speed[0] = vx;
	shapeArray[size - 1].speed[1] = vy;
	shapeArray[size - 1].speed[2] = vz;
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


//movement
/*
*Shape Mover
- moves the shape at index by changing its model matrix
- and center pos
*/
void DynamicShapeArray::Move(int index) {
	float* speed = shapeArray[index].speed;
	float size = shapeArray[index].size;
	if (speed[0] || speed[1] || speed[2]) {
		CheckCollision(index);
		shapeArray[index].Model = glm::translate(glm::mat4(1.0f), glm::vec3(speed[0] * (speedUP * globalSpeed), speed[1] * (speedUP * globalSpeed), speed[2] * (speedUP * globalSpeed))) * shapeArray[index].Model;
		shapeArray[index].center[0] += speed[0] * (speedUP * globalSpeed);
		shapeArray[index].center[1] += speed[1] * (speedUP * globalSpeed);
		shapeArray[index].center[2] += speed[2] * (speedUP * globalSpeed);
	}
}

/*
*Sphere Mover
-Handles Sphere movement because it needs to be moved by input
*/

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
}

/*
*Speed modifier
*/
void DynamicShapeArray::SpeedUP(bool up) {

	if (speedUP < MAX_SPEEDUP && up) {
		speedUP++;
	}
	else if (speedUP > 0 && !up) {
		speedUP--;
	}
	std::cout << "Speed modifier: " << speedUP << " (Default: 50, Max: " << MAX_SPEEDUP <<")" << std::endl;
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
	case T_RING:
		return 2 * 8 * CIRCLE_TRIANGLE_NUM * 3;
	}
	return 0;
}


//Setters

/*
*Simple Color Setter
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


void DynamicShapeArray::SetRandomColor(int index, float alpha_value) {
	float r = RandomFloat(0.0, 1.0);
	float g = RandomFloat(0.0, 1.0);
	float b = RandomFloat(0.0, 1.0);
	SetColor(index, r, g, b, alpha_value);
}


//private functions


/*
*Handling Collisions
-handles collisions and changes speeds when it happens
*/
void DynamicShapeArray::CheckCollision(int index) {
	if (index >= size || index < 2) {
		return;
	}
	bool hasCollision;
	float bigPos[] = { shapeArray[index].center[0] + shapeArray[index].speed[0],
		shapeArray[index].center[1] + shapeArray[index].speed[1],
		shapeArray[index].center[2] + shapeArray[index].speed[2] };
	float nextPos1[3];

	float* pos, * pos1;
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
			if (shapeArray[j].shapeType == T_RING || shapeArray[j].shapeType == T_SPHERE || (shapeArray[s].shapeType == T_CUBE && shapeArray[j].shapeType == T_CYLINDER)) {
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
					hasCollision = (dsqr <= (size0 / 2 + size1 / 2) * (size0 / 2 + size1 / 2)) && (dsqr >= size1 * size1 / 4);
				}
				else if (shapeArray[j].shapeType == T_CUBE) {

					if (dx >= (size1 / 2 + size0 / 2)) { hasCollision = false; }
					else if (dy >= (size1 / 2 + size0 / 2)) { hasCollision = false; }
					else if (dz >= (size1 / 2 + size0 / 2)) { hasCollision = false; }
					//be completely in
					else if ((dx < abs((size1 - size0) / 2)) && (dy < abs((size1 - size0) / 2)) && (dz < abs((size1 - size0) / 2))) { hasCollision = false; }

					else if (dx < (size1 / 2)) { hasCollision = true; }
					else if (dy < (size1 / 2)) { hasCollision = true; }
					else if (dz < (size1 / 2)) { hasCollision = true; }
					else {
						float cornerDistance_sq = ((dx - size1 / 2) * (dx - size1 / 2)) +
							((dy - size1 / 2) * (dy - size1 / 2)) +
							((dz - size1 / 2) * (dz - size1 / 2));
						hasCollision = (cornerDistance_sq < (size0 / 2 * size0 / 2));
					}

				}
				else if (shapeArray[j].shapeType == T_CYLINDER) {
					dsqr = dx * dx + dz * dz;

					if (dx > (size1 / 2 + size0 / 2)) { hasCollision = false; }
					else if (dy > (size1 / 2 + size0 / 2)) { hasCollision = false; }
					else if (dz > (size1 / 2 + size0 / 2)) { hasCollision = false; }
					else if ((dsqr <= (((size0 / 2) + (size1 / 2)) * ((size1 / 2) + (size0 / 2))) && dy <= (size1 / 2))) { hasCollision = true; }
					else {
						dsqr = dx * dx + dy * dy + dz * dz;
						hasCollision = (dsqr <= (size0 / 2 + SQRT_2 * size1 / 2) * (size0 / 2 + SQRT_2 * size1 / 2));
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
					if (dsqr > (size1 / 2 + size0 / 2) * (size1 / 2 + size0 / 2)) { hasCollision = false; }
					else if (dy >= (size1 / 2 + size0 / 2)) { hasCollision = false; }
					else if (dy < (size1 / 2)) { hasCollision = true; }
					else {
						dsqr = dx * dx + dy * dy + dz * dz;
						hasCollision = dsqr <= (SQRT_2 * size0 / 2 + SQRT_2 * size1 / 2) * (SQRT_2 * size0 + SQRT_2 * size1 / 2);
					}
				}
			}
			else if (shapeArray[s].shapeType == T_RING) {
				float size00 = shapeArray[s].d2, radi = size0 - (2 * size00), radB = size0;

				if (shapeArray[j].shapeType == T_CUBE) {
					if (dx >= (size1 / 2 + size0)) { hasCollision = false; }
					else if (dy >= (size1 / 2 + size0)) { hasCollision = false; }
					else if (dz >= (size1 / 2 + size0)) { hasCollision = false; }
					//be completely in 
					else if ((dx < abs((size1 / 2) - (size0)) && (dy < abs((size1 / 2) - (size0))) && (dz < abs((size1 / 2) - (size0))))) { hasCollision = false; }

					else if (dx < (size1 / 2)) { hasCollision = true; }
					else if (dy < (size1 / 2) + size00) { hasCollision = true; }
					else if (dz < (size1 / 2)) { hasCollision = true; }
					else {
						float cornerDistance_sq = ((dx - size1 / 2) * (dx - size1 / 2)) +
							((dy - size1 / 2) * (dy - size1 / 2)) +
							((dz - size1 / 2) * (dz - size1 / 2));
						hasCollision = true;
					}

				}
				else if (shapeArray[j].shapeType == T_CYLINDER) {
					hasCollision = false;
				}
				else if (shapeArray[j].shapeType == T_SPHERE) {
					dsqr = dx * dx + dz * dz;

					if (dx > (size1 / 2 + size0)) { hasCollision = false; }
					else if (dy > (size1 / 2 + size0)) { hasCollision = false; }
					else if (dz > (size1 / 2 + size0)) { hasCollision = false; }
					else if ((dsqr <= (((size0)+(size1 / 2)) * ((size1 / 2) + (size0))) && dy <= (size1 / 2))) { hasCollision = true; }
					else {
						dsqr = dx * dx + dy * dy + dz * dz;
						hasCollision = (dsqr <= (size0 / 2 + SQRT_2 * size1 / 2) * (size0 / 2 + SQRT_2 * size1 / 2));
					}
				}
				else if (shapeArray[j].shapeType == T_RING) {
					hasCollision = false;
				}
			}

			if (hasCollision) {
				Collide(s, j);
				Collide(j, s);
#ifdef _WIN32
				if (index > 1 && soundsEnabled) {
					PlaySound(TEXT("collision.wav"), NULL, SND_FILENAME | SND_ASYNC);
				}
#endif
			}
		}
	}
}

/*
*Collision
-changes the speeds of the shapes in a collision
*/
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

		glm::vec3 centerToCenter(dx, dy, dz);
		centerToCenter = glm::normalize(centerToCenter);
		Tspeed = glm::normalize(speed);

		speed = glm::length(speed) * normalize((-centerToCenter) * glm::dot(Tspeed, centerToCenter));

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
		float dists[2] = { cos((acos(abs(glm::dot(centerToCenter,X))) + acos(abs(glm::dot(centerToCenter,Z)))) / 2),  glm::dot(centerToCenter,Y) };
		float m = abs(dists[1]);
		m = std::max(m, abs(dists[0]));

		glm::vec3 M;

		if ((dists[1] >= SQRT_2 / 2 && dists[1] < 1) || (dists[1] <= -SQRT_2 / 2 && dists[1] > -1)) {
			shapeArray[index2].speed[1] = -speed[1];

		}
		else if (dy < size1) {

			Tspeed = glm::length(speed) * normalize((-centerToCenter) * glm::dot(Tspeed, centerToCenter));
			shapeArray[index2].speed[0] = Tspeed[0];
			shapeArray[index2].speed[2] = Tspeed[2];
		}
	}
	if (shapeType1 == T_RING) {
		glm::vec3 X(1.0f, 0.0f, 0.0f);
		glm::vec3 Y(0.0f, 1.0f, 0.0f);
		glm::vec3 Z(0.0f, 0.0f, 1.0f);

		float s1 = shapeArray[index1].d;
		float s2 = shapeArray[index1].d2;


		glm::vec3 centerToCenter(dx - s1 - s2, dy, dz - s1 - s2);
		float size1 = shapeArray[index1].size / 2;
		float size2 = shapeArray[index2].size / 2;
		Tspeed = glm::normalize(speed);
		centerToCenter = glm::normalize(centerToCenter);
		glm::vec3 ctc2(centerToCenter[0], 0, centerToCenter[2]);
		float dists[2] = { cos((acos(abs(glm::dot(centerToCenter,X))) + acos(abs(glm::dot(centerToCenter,Z)))) / 2),  glm::dot(centerToCenter,Y) };
		float m = abs(dists[1]);
		m = std::max(m, abs(dists[0]));

		glm::vec3 M;

		if ((dists[1] >= SQRT_2 / 2 && dists[1] < 1) || (dists[1] <= -SQRT_2 / 2 && dists[1] > -1)) {
			shapeArray[index2].speed[1] = -speed[1];

		}
		else if (dy < size1) {

			Tspeed = glm::length(speed) * normalize((-centerToCenter) * glm::dot(Tspeed, centerToCenter));
			shapeArray[index2].speed[1] = Tspeed[1];
			shapeArray[index2].speed[0] = Tspeed[0];
			shapeArray[index2].speed[2] = Tspeed[2];
		}
	}
}

//Creating Shapes
//Cube
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
	AddShape(positions, 8 * 3, T_CUBE, x0+size/2, y0 + size / 2, z0 + size / 2, size);
}

//Sphere
void DynamicShapeArray::CreateSphere(float x0, float y0, float z0, float radius) {

	float x, y, z, xy;                              // vertex position
	float nx, ny, nz, lengthInv = 1.0f / radius;    // vertex normal

	float sectorStep = 2 * PI / SPHERE_SECTOR_NUM;
	float stackStep = PI / SPHERE_STACK_NUM;
	float sectorAngle, stackAngle;
	float points[(SPHERE_SECTOR_NUM + 1) * (SPHERE_STACK_NUM + 1) * 3];

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
			if (firstSphere) {
				// calculating normals
				nx = x * lengthInv;
				ny = y * lengthInv;
				nz = z * lengthInv;

				sphere_normals[n] = nx;
				sphere_normals[n + 1] = ny;
				sphere_normals[n + 2] = nz;
			}
			n += 3;
		}
	} 
	AddShape(points, (SPHERE_SECTOR_NUM + 1) * (SPHERE_STACK_NUM + 1) * 3, T_SPHERE, x0, y0, z0, 2*radius);
	firstSphere = false;
}

//Cylinder
void DynamicShapeArray::CreateCylinder(float x, float y, float z, float radius, float height) {
	float* circle1, * circle2;
	circle1 = CreateCircle(x, y, z, radius);
	circle2 = CreateCircle(x, (y + height), z, radius);
	float cylinder_pos[216];
	for (int i = 0; i < 108; i++) {
		cylinder_pos[i] = circle1[i];
		cylinder_pos[i + 108] = circle2[i];
	}
	free(circle1);
	free(circle2);

	if (firstCylinder) {

		cylinder_normals[0] = cylinder_normals[2] = cylinder_normals[108] = cylinder_normals[110] = 0;
		cylinder_normals[1] = -34 * cos((2 * PI) / 34);
		cylinder_normals[109] = 34 * cos((2 * PI) / 34);

		for (int i = 1, n = 1; i < 36; i++) {
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

	AddShape(cylinder_pos, 216, T_CYLINDER, x, y + height / 2, z, 2 * radius);
}

//Ring
void DynamicShapeArray::CreateRing(float x,float y, float z, float r1, float r2) {
	float * circle1 = CreateCircle(x, y+r2, z, abs(r1-r2));
	float* circle2 = CreateCircle(x, y, z, abs(r1 - 2*r2));
	float* circle3 = CreateCircle(x, y-r2, z, abs(r1 - r2));
	float* circle4 = CreateCircle(x, y, z, r1);

	float* circle1b = CreateCircle(x, y + r2 / 2, z, abs(r1 - 3 *r2 / 2));
	float* circle2b = CreateCircle(x, y - r2 / 2, z, abs(r1 - 3*r2 / 2));

	float* circle3b = CreateCircle(x, y - SQRT_2 * r2 / 2, z, abs(r1 - r2 / 2));
	float* circle4b = CreateCircle(x, y + SQRT_2 * r2 / 2, z, abs(r1 - r2 / 2));

	int vertex_num = (CIRCLE_VERTEX_NUM -1);
	int vertex_size = (CIRCLE_VERTEX_NUM - 1) * 3;
	float ringVertices[(CIRCLE_VERTEX_NUM - 1)*8 * 3];

	for (int i = 3,n=0; n < vertex_size; i+=3) {
		ringVertices[n] = circle1[i];
		ringVertices[n+1] = circle1[i+1];
		ringVertices[n+2] = circle1[i+2];

		ringVertices[n + vertex_size] = circle1b[i];
		ringVertices[n + vertex_size + 1] = circle1b[i + 1];
		ringVertices[n + vertex_size + 2] = circle1b[i + 2];
		
		ringVertices[n + 2 * vertex_size] = circle2[i];
		ringVertices[n + 2 * vertex_size + 1] = circle2[i + 1];
		ringVertices[n + 2 * vertex_size + 2] = circle2[i + 2];

		ringVertices[n + 3 * vertex_size] = circle2b[i];
		ringVertices[n + 3 * vertex_size + 1] = circle2b[i + 1];
		ringVertices[n + 3 * vertex_size + 2] = circle2b[i + 2];
		
		ringVertices[n + 4 * vertex_size] = circle3[i];
		ringVertices[n + 4 * vertex_size + 1] = circle3[i + 1];
		ringVertices[n + 4 * vertex_size + 2] = circle3[i + 2];
		
		ringVertices[n + 5 * vertex_size] = circle3b[i];
		ringVertices[n + 5 * vertex_size + 1] = circle3b[i + 1];
		ringVertices[n + 5 * vertex_size + 2] = circle3b[i + 2];

		ringVertices[n + 6 * vertex_size] = circle4[i];
		ringVertices[n + 6 * vertex_size + 1] = circle4[i + 1];
		ringVertices[n + 6 * vertex_size + 2] = circle4[i + 2];

		ringVertices[n + 7 * vertex_size] = circle4b[i];
		ringVertices[n + 7 * vertex_size + 1] = circle4b[i + 1];
		ringVertices[n + 7 * vertex_size + 2] = circle4b[i + 2];

		if (firstRing) {
			ring_normals[n] = ring_normals[n + 2] = 0.0f;
			ring_normals[n + 1] = 1.0f;

			ring_normals[n + vertex_size] = -cos(2 * PI * i / (34 * 3));
			ring_normals[n + vertex_size + 1] = SQRT_2/2;
			ring_normals[n + vertex_size + 2] = -sin(2 * PI * i / (34 * 3));

			ring_normals[n + 2 * vertex_size] = -cos(2 * PI * i / (34 * 3));
			ring_normals[n + 2 * vertex_size + 1] = 0.0f;
			ring_normals[n + 2 * vertex_size + 2] = -sin(2 * PI * i / (34 * 3));

			ring_normals[n + 3 * vertex_size] = -cos(2 * PI * i / (34 * 3));
			ring_normals[n + 3 * vertex_size + 1] = -SQRT_2 / 2;
			ring_normals[n + 3 * vertex_size + 2] = -sin(2 * PI * i / (34 * 3));

			ring_normals[n + 4 * vertex_size] = 0.0f;
			ring_normals[n + 4 * vertex_size + 1] = -1.0f;
			ring_normals[n + 4 * vertex_size + 2] = 0.0f;

			ring_normals[n + 5 * vertex_size] = cos(2 * PI * i / (34 * 3));
			ring_normals[n + 5 * vertex_size + 1] = -SQRT_2 / 2;
			ring_normals[n + 5 * vertex_size + 2] = sin(2 * PI * i / (34 * 3));

			ring_normals[n + 6 * vertex_size] = cos(2 * PI * i / (34 * 3));
			ring_normals[n + 6 * vertex_size + 1] = 0.0f;
			ring_normals[n + 6 * vertex_size + 2] = sin(2 * PI * i / (34 * 3));

			ring_normals[n + 7 * vertex_size] = cos(2 * PI * i / (34 * 3));
			ring_normals[n + 7 * vertex_size + 1] = SQRT_2 / 2;
			ring_normals[n + 7 * vertex_size + 2] = sin(2 * PI * i / (34 * 3));
		}
		n +=3;
	}
	/*
		1/\8
		2||7
		3||6
		4\/5
	*/
	if (firstRing) {
		int sector_num = 8;
		for (int sector = 0, pos = 0; sector < sector_num; sector++) {
			for (int i = 0; i < CIRCLE_TRIANGLE_NUM; i++) {
				ring_indices[pos++] = i + (sector % sector_num) * vertex_num;
				ring_indices[pos++] = i + ((sector + 1) % sector_num) * vertex_num;
				ring_indices[pos++] = i + 1 + (sector % sector_num) * vertex_num;
				ring_indices[pos++] = i + ((sector + 1) % sector_num) * vertex_num;
				ring_indices[pos++] = i + 1 + ((sector + 1) % sector_num) * vertex_num;
				ring_indices[pos++] = i + 1 + (sector % sector_num) * vertex_num;
			}
		}
	}
	firstRing = false;
	AddShape(ringVertices, 8*vertex_size, T_RING, x, y, z, r1);
	shapeArray[size-1].d2 = r2;
	free(circle1);
	free(circle2);
	free(circle3);
	free(circle4);
	free(circle1b);
	free(circle2b);
	free(circle3b);
	free(circle4b);
}

//Circle helps with Cylinder and Ring
float* DynamicShapeArray::CreateCircle(float x, float y, float z, float radius) {
	int num_of_sides = CIRCLE_TRIANGLE_NUM;
	int num_of_vertices = num_of_sides + 2;
	int n = 3 * num_of_vertices;
	float twicePi = 2.0f * PI;
	float * vertices = (float *) malloc(sizeof(float) * (CIRCLE_TRIANGLE_NUM+2)*3);
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

//creates buffers for each shape
void DynamicShapeArray::createBuffer(int index) {
	unsigned int buffer_id;
	float * shape = shapeArray[index].data;
	int shape_size = shapeArray[index].size;
	int index_pointer_size = GetIndexPointerSize(index);
	int normal_pointer_size;
	float* normals = GetNormals(shapeArray[index].shapeType);
	if (shapeArray[index].shapeType == T_CUBE) {
		normal_pointer_size = 24;
	}
	else if (shapeArray[index].shapeType == T_SPHERE) {
		normal_pointer_size = 2109;
	}
	else if (shapeArray[index].shapeType == T_CYLINDER) {
		normal_pointer_size = 216;
	}
	else if (shapeArray[index].shapeType == T_RING) {
		normal_pointer_size = 8 * (CIRCLE_VERTEX_NUM-1) * 3;
	}
	unsigned int * index_array = GetIndexPointer(index);

	//create and bind the vao
	unsigned int vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	
	//create a buffer to keep out positions
	glGenBuffers(1, &buffer_id);
	glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
	glBufferData(GL_ARRAY_BUFFER, shape_size * sizeof(float) + normal_pointer_size * sizeof(float), 0, GL_STATIC_DRAW);
	
	glBufferSubData(GL_ARRAY_BUFFER, 0, shape_size * sizeof(float), shape);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, 0);
	
	glBufferSubData(GL_ARRAY_BUFFER, shape_size * sizeof(float), normal_pointer_size * sizeof(float), normals);
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
}

//Initialize Index Arrays
//fills sphere_indices
void DynamicShapeArray::InitSphereIndices() {
	unsigned int k1, k2;

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

//Cylinder
void DynamicShapeArray::InitCylinderIndices() {
	int offset = CIRCLE_TRIANGLE_NUM + 2;

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



//Assisting functions
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

//returns shape normal array pointer for each shape
//getters to help when we need a pointer to a shape's normals or indices
float* DynamicShapeArray::GetNormals(int shapeType) {
	switch (shapeType)
	{
	case T_CUBE:
		return cube_normals;
	case T_SPHERE:
		return sphere_normals;
	case T_CYLINDER:
		return cylinder_normals;
	case T_RING:
		return ring_normals;
	}
	return nullptr;
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
	case T_RING:
		return &ring_indices[0];
	}
	return nullptr;
}

//Adds a shape to shapeArray
void DynamicShapeArray::AddShape(float * element, int elementSize, int shapeType, float x0, float y0, float z0, float d) {
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

		createBuffer(index);
	} else {
		std::cout << "Error: Could not Add Shape in Array" << std::endl;

	}
}

//Random number generators
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
	return (float)distributionDouble(generator);
}