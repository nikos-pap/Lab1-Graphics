#include "DynamicShapeArray.h"
#include "Shader.h"
#include <glm/gtc/matrix_transform.hpp>

DynamicShapeArray shapeArray;
//camera 
glm::vec3 cameraPos(-200.0f, 200.0f, 200.0f);
glm::vec3 cameraFront(1.0f, -1.0f, -1.0f);
glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);
glm::mat4 View = glm::lookAt(
	cameraPos, // Camera is at (100,150,100), in World Space
	//glm::vec3(0.0f, 150.0f, 0.0f),
	(cameraPos + cameraFront), // and looks at the origin
	cameraUp  // Head is up (set to 0,-1,0 to look upside-down)
);

//speeds
float cameraSpeed = 20.0f / GLOBAL_SPEED;
float yawSpeed = 1.0f / GLOBAL_SPEED;
bool spaceChecker = true;
bool joystick_space = false;

int processCameraMovement(GLFWwindow* window) {
	int present = glfwJoystickPresent(GLFW_JOYSTICK_1);
	if (1 == present)
	{
		int axesCount;
		const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axesCount);
		//std::cout << count << std::endl;

		/*
		std::cout << "Left Stick X Axis: " << axes[0] << std::endl; // tested with PS4 controller connected via micro USB cable
		std::cout << "Left Stick Y Axis: " << axes[1] << std::endl; // tested with PS4 controller connected via micro USB cable
		std::cout << "Right Stick X Axis: " << axes[2] << std::endl; // tested with PS4 controller connected via micro USB cable
		std::cout << "Right Stick Y Axis: " << axes[3] << std::endl; // tested with PS4 controller connected via micro USB cable
		std::cout << "Left Trigger/L2: " << axes[4] << std::endl; // tested with PS4 controller connected via micro USB cable
		std::cout << "Right Trigger/R2: " << axes[5] << std::endl; // tested with PS4 controller connected via micro USB cable
		/**/

		int buttonCount;
		const unsigned char* buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttonCount);
		if (GLFW_PRESS == buttons[1] && spaceChecker)
		{
			std::cout << "joystick_space: " << joystick_space  << std::endl;
			joystick_space = true;
			spaceChecker = false;
			shapeArray.CreateRandomShape();
		}
		else if (GLFW_RELEASE == buttons[1] && joystick_space){
			joystick_space = false;
			spaceChecker = true;
		}

		if (GLFW_PRESS == buttons[0]) {
			return GLFW_PRESS;
		}

		const char* name = glfwGetJoystickName(GLFW_JOYSTICK_1);
		//std::cout << name << std::endl;
		/*if (axes[5] >= -1)//R2
			cameraPos += (axes[5] + 1) * cameraFront;
		if (axes[4] >= -1)//L2
			cameraPos -= (axes[4] + 1) * cameraFront;*/
		if (axes[3] >= -1)//RY
			cameraPos -= (axes[3]) * cameraFront;
		if (abs(axes[5]) >= 0.2)//R2
			cameraPos += (axes[5] + 1) * cameraUp;
		if (abs(axes[4]) >= 0.2)//L2
			cameraPos -= (axes[4] + 1) * cameraUp;
		/*if (abs(axes[3]) >= 0.2)//RY
			cameraPos -= (axes[3] / 1) * cameraUp;*/
		if (abs(axes[2]) >= 0.2)//RX
			cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * (axes[2]);
		if (abs(axes[1]) >= 0.2)//LY
			cameraFront -= (axes[1]/30) * cameraUp;
		if (abs(axes[0]) >= 0.2)//LX
			cameraFront += glm::normalize(glm::cross(cameraFront, cameraUp)) * (axes[0] / 30);
		if (buttons[11] == GLFW_PRESS)
			shapeArray.MoveSphere(1, glm::vec3(1.0f, 0.0f, 0.0f));
		if (buttons[13] == GLFW_PRESS)
			shapeArray.MoveSphere(1, glm::vec3(-1.0f, 0.0f, 0.0f));
		if (buttons[10] == GLFW_PRESS)
			shapeArray.MoveSphere(1, glm::vec3(0.0f, 1.0f, 0.0f));
		if (buttons[12] == GLFW_PRESS)
			shapeArray.MoveSphere(1, glm::vec3(0.0f, -1.0f, 0.0f));
		if (buttons[4] == GLFW_PRESS)
			shapeArray.MoveSphere(1, glm::vec3(0.0f, 0.0f, 1.0f));
		if (buttons[5] == GLFW_PRESS)
			shapeArray.MoveSphere(1, glm::vec3(0.0f, 0.0f, -1.0f));

	}

	if ((glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) && spaceChecker && !joystick_space) {
		spaceChecker = false;
		shapeArray.CreateRandomShape();
	}
	else if ((glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) && !joystick_space) {
		spaceChecker = true;
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		cameraPos += cameraUp * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
		cameraPos -= cameraUp * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
		cameraFront += yawSpeed * cameraUp;
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
		cameraFront -= yawSpeed * cameraUp;
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
		cameraFront -= glm::normalize(glm::cross(cameraFront, cameraUp)) * (yawSpeed / 2);
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
		cameraFront += glm::normalize(glm::cross(cameraFront, cameraUp)) * (yawSpeed / 2);

	//Sphere Controls
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		shapeArray.MoveSphere(1, glm::vec3(1.0f, 0.0f, 0.0f));
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		shapeArray.MoveSphere(1, glm::vec3(-1.0f, 0.0f, 0.0f));
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		shapeArray.MoveSphere(1, glm::vec3(0.0f, 1.0f, 0.0f));
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		shapeArray.MoveSphere(1, glm::vec3(0.0f, -1.0f, 0.0f));

	View = glm::lookAt(
		cameraPos, // Camera is at (100,150,100), in World Space
		//glm::vec3(0.0f, 150.0f, 0.0f),
		(cameraPos + cameraFront), // and looks at the origin
		cameraUp  // Head is up (set to 0,-1,0 to look upside-down)
	);
	return glfwGetKey(window, GLFW_KEY_ESCAPE);
}


int main(void) {
	GLFWwindow* window;

	if (!glfwInit()) {
		return -1;
	}


	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(600, 600, u8"Συγκρουόμενα Φούντο γαμιέσαι", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}


	glfwMakeContextCurrent(window);

	/* need to do this after I have a valid context */
	if (glewInit() != GLEW_OK)
		std::cout << "Error!" << std::endl;
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	std::cout << glGetString(GL_VERSION) << std::endl;
	glm::mat4 Projection = glm::perspective(glm::radians(40.0f),1.0f,0.001f,1000.0f); //ortho coords: -200.0f, 200.0f, -200.0f, 150.0f, 250.0f, 0.0f



	// Model matrix : an identity matrix (model will be at the origin)
	glm::mat4 Model = glm::mat4(1.0f);

	// Our ModelViewProjection : multiplication of our 3 matrices
	glm::mat4 MVP;// = Projection * View * Model; // Remember, matrix multiplication is the other way around
	//MVP = glm::mat4(1.0f);
	//glfwSwapInterval(1);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_BLEND);
	glEnable(GL_NORMALIZE);
	//glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
	//glEnable(GL_LIGHTING);
	//glEnable(GL_LIGHT0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_TEXTURE_2D);

	shapeArray.CreateShape(0.0f, 0.0f, 0.0f, 100.0f, T_CUBE);
	shapeArray.SetColor(0, 0.0f, 0.0f, 1.0f, 0.5f);
	shapeArray.CreateShape(35.0f, 35.0f, 35.0f, 30.0f, T_CUBE);
	shapeArray.SetColor(1, 1.0f, 0.0f, 0.1f, 1.0f);
	/*shapeArray.CreateShape(0.0f, 100.0f, 100.0f, 10.0f, T_CYLINDER);
	shapeArray.SetColor(1, 0.0f, 0.7f, 0.5f, 1.0f);*/

	Shader shader("Shader.shader");
	//glm::vec3 lightPos = glm::vec3(150.0f, 150.0f, 150.0f);//not used consider removing
	float light[3] = { 150.0f,-100000.0f,50.0f};
	shader.SetUniform3f("u_Light", light);
	shader.SetUniformMat4f("model", Model);
	unsigned int ib_size;
	int shapeArrSize;
	


	while (processCameraMovement(window) != GLFW_PRESS && !glfwWindowShouldClose(window)) {
		shader.Bind();
		shapeArrSize = shapeArray.GetSize();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		/*if ((glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)&&spaceChecker) {
			spaceChecker = false;
			shapeArray.CreateRandomShape();
		}
		else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) {
			spaceChecker = true;
		}*/
		
		//processCameraMovement(window);
		MVP = Projection * View * Model;

		for (int i = shapeArrSize-1; i >= 0; i--) {
			float* color = shapeArray.GetColor(i);
			ib_size = shapeArray.GetIndexPointerSize(i);
			shapeArray.BindShape(i);
			//std::cout << "3" << std::endl;
			shapeArray.CheckCollision(2);
			shapeArray.Move(i);
			Model = shapeArray.GetModel(i);
			MVP = Projection * View * Model;
			shader.SetUniformMat4f("u_MVP", MVP);
			shader.SetUniform4f("u_Color",color);
			shader.SetUniformMat4f("model", View);
			//shader.SetUniform3f("u_Light", 150.0f, 150.0f, 150.0f);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			/*if (i == 1) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			}
			else {
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}*/
			//std::cout << shapeArray.GetSpeed(i)[0] << shapeArray.GetSpeed(i)[1] << shapeArray.GetSpeed(i)[2] << std::endl;
			glDrawElements(GL_TRIANGLES, ib_size, GL_UNSIGNED_INT, nullptr);
			//glBindVertexArray(0); //it works without this*/
		}

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
		shader.Unbind();
	}
	Model = glm::mat4(1.0f);
	//glDeleteProgram(shader);
	glfwTerminate();

	return 0;
}