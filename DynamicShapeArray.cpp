#include "DynamicShapeArray.h"

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
		2, 6, 0 //bottom
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
	shapeFactory = new ShapeFactory();
	capacity = 10;
	shapeArray = (Shape *) malloc(capacity * sizeof(Shape));
	size = 0;
}

/*
*Simple Destructor
-freeing the dynamically allocated space
*/
//remember to free the rest
DynamicShapeArray::~DynamicShapeArray() {
	free(shapeArray);
}

/*
*Random Shape creator
-generates random shapes and adds them to the ShapeArray
- Deprecated. Moved to ShapeFactory.
*/
void DynamicShapeArray::CreateRandomShape() {
	AddShape(shapeFactory->CreateRandomShape());

}

/*
Shape Creator
-creates new shape to add to the Array
- Deprecated. Moved to ShapeFactory.
*/
void DynamicShapeArray::CreateShape(float x, float y, float z, int elementSize, int ShapeType) {
	AddShape(shapeFactory->CreateShape(x, y, z, elementSize, ShapeType));
}

//Assisting functions
void DynamicShapeArray::Extend()
{
	capacity += 10;
	Shape* temp = (Shape*)realloc(shapeArray, (capacity) * sizeof(Shape));
	if (temp != nullptr) {
		for (int i = 0; i < size; i++) {
			temp[i] = shapeArray[i];
		}
		shapeArray = temp;
	}
}


//Adds a shape to shapeArray
void DynamicShapeArray::AddShape(Shape shape) {
	if (capacity == size) {
		Extend();
	}
	shapeArray[size] = shape;
	size++;
}


void DynamicShapeArray::SetRandomColor(int index, float alpha_value) {
	shapeFactory->SetRandomColor(shapeArray[index], alpha_value);
}

void DynamicShapeArray::SetColor(int index, float r_value, float g_value, float b_value, float alpha_value) {
	shapeFactory->SetColor(shapeArray[index], r_value, g_value, b_value, alpha_value);
}

float* DynamicShapeArray::GetColor(int index) {
	if (index < size) {
		return shapeFactory->GetColor(shapeArray[index]);
	}
	else return nullptr;
}

int DynamicShapeArray::GetIndexPointerSize(int index) {
	return shapeFactory->GetIndexPointerSize(shapeArray[index].shapeType);
}

float* DynamicShapeArray::GetNormals(int shapeType) {
	return shapeFactory->GetNormals(shapeType);
}

/*
Buffer Binder
- binds shape's vao and ibo
- created mainly because it removes 2-3 Getters/Setters
*/
void DynamicShapeArray::BindShape(int index) {
	shapeFactory->BindShape(shapeArray[index]);
}


//movement
/*
*Shape Mover
- moves the shape at index by changing its model matrix
- and center pos
*/
void DynamicShapeArray::Move(int index) {
	float* speed = shapeArray[index].speed;
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
		std::cout << "Speed modifier: " << speedUP << " (Default: 50, Max: " << MAX_SPEEDUP << ")" << std::endl;
	}
	else if (speedUP > 0 && !up) {
		speedUP--;
		std::cout << "Speed modifier: " << speedUP << " (Default: 50, Max: " << MAX_SPEEDUP << ")" << std::endl;
	}
	
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
		if (i == index)
			continue;
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
			if (size0 <= 10 && soundsEnabled) {
				PlaySound(TEXT("collision.wav"), NULL, SND_FILENAME | SND_ASYNC);
			}
#endif
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
