#include "DynamicShapeArray.h"
#include "Shader.h"
#include <glm/gtc/matrix_transform.hpp>

#ifdef _WIN32
	#include <Windows.h>
#endif
#define STB_IMAGE_IMPLEMENTATION   
#include "stb_image.h"
#pragma comment(lib, "Winmm.lib")

DynamicShapeArray shapeArray;
//camera 
glm::vec3 cameraPos(-200.0f, 200.0f, 200.0f);
glm::vec3 cameraFront(2.0f, -1.2f, -1.2f);
glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);
glm::mat4 View = glm::lookAt(
	cameraPos, // Camera is at (100,150,100), in World Space
	(cameraPos + cameraFront), // and looks at the origin
	cameraUp  // Head is up (set to 0,-1,0 to look upside-down)
);

//speeds
float cameraSpeed = 20.0f / GLOBAL_SPEED;
float yawSpeed = 1.0f / GLOBAL_SPEED;
bool spaceChecker = true;
bool joystick_space = false;
bool tex = true;
bool texChecker = true;


unsigned int loadTexture()
{
	unsigned int texture;
	glGenTextures(1, &texture);
	//glBindTexture(GL_TEXTURE_2D, texture);
	// set the texture wrapping/filtering options (on the currently bound texture object)
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//generating cube map
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

	//Define all 6 faces
	// load and generate the texture
	int width, height, nrChannels;
	unsigned char* data = stbi_load("texture.jpg", &width, &height, &nrChannels, STBI_rgb_alpha);
	if (data)
	{
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		//glGenerateMipmap(GL_TEXTURE_2D);
		//std::cout << "ax koula1" << std::endl;
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		//std::cout << "ax koula11" << std::endl;
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
		//std::cout << "ax koula111" << std::endl;
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 0);



	stbi_image_free(data);

    return texture;
}





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
		if (abs(axes[5]) >= 0.2)//RY//LY//R2
			cameraPos -= (axes[5]) * cameraFront;
		if (abs(axes[1]) >= 0.2)//R2//LY
			cameraPos += (axes[1] + 1) * cameraUp;
		if (abs(axes[4]) >= 0.2)//L2
			cameraPos -= (axes[4] + 1) * cameraUp;
		if (abs(axes[0]) >= 0.2)//RX
			cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * (axes[0]);
		if (abs(axes[3]) >= 0.2)//LY//RY//L2
			cameraFront -= (axes[3]/30) * cameraUp;
		if (abs(axes[2]) >= 0.2)//LX//RX
			cameraFront += glm::normalize(glm::cross(cameraFront, cameraUp)) * (axes[2] / 30);
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
	if ((glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) && texChecker) {
		texChecker = false;
		tex = !tex;		
	}
	else if ((glfwGetKey(window, GLFW_KEY_T) == GLFW_RELEASE)) {
		texChecker = true;
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
		cameraFront -= glm::normalize(glm::cross(normalize(cameraFront), cameraUp)) * (yawSpeed / 2);
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
		cameraFront += glm::normalize(glm::cross(normalize(cameraFront), cameraUp)) * (yawSpeed / 2);

	//Sphere Controls
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		shapeArray.MoveSphere(1, glm::vec3(1.0f, 0.0f, 0.0f));
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		shapeArray.MoveSphere(1, glm::vec3(-1.0f, 0.0f, 0.0f));
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		shapeArray.MoveSphere(1, glm::vec3(0.0f, 1.0f, 0.0f));
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		shapeArray.MoveSphere(1, glm::vec3(0.0f, -1.0f, 0.0f));
	if (glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS)
		shapeArray.MoveSphere(1, glm::vec3(0.0f, 0.0f, -1.0f));
	if (glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS)
		shapeArray.MoveSphere(1, glm::vec3(0.0f, 0.0f, 1.0f));

	View = glm::lookAt(
		cameraPos, // Camera is at (100,150,100), in World Space
		(cameraPos + cameraFront), // and looks at the origin
		cameraUp  // Head is up (set to 0,-1,0 to look upside-down)
	);

	return glfwGetKey(window, GLFW_KEY_ESCAPE);
}


int main(void) {
	GLFWwindow* window;
	unsigned int textureID;

	if (!glfwInit()) {
		return -1;
	}


	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(600, 600, u8"Συγκρουόμενα", NULL, NULL);
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



	// Model Matrix
	glm::mat4 Model = glm::mat4(1.0f);

	//ModelViewProjection Matrix
	glm::mat4 MVP;// = Projection * View * Model;

	//glfwSwapInterval(1);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_BLEND);
	glEnable(GL_NORMALIZE);
	//glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
	//glEnable(GL_LIGHTING);
	//glEnable(GL_LIGHT0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//for the textures
	glDisable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);


	shapeArray.CreateShape(0.0f, 0.0f, 0.0f, 100.0f, T_CUBE);
	shapeArray.SetRandomColor(0, 0.5f);
	shapeArray.CreateShape(35.0f, 35.0f, 35.0f, 30.0f, T_SPHERE);
	shapeArray.SetColor(1, 1.0f, 1.0f, 1.0f, 1.0f);

	Shader shader("Shader.shader");
	shader.SetUniformMat4f("model", Model);
	unsigned int ib_size;
	int shapeArrSize;
	float x = 1.0f;
	float l = 1.0f;
	textureID = loadTexture();
#ifdef _WIN32
	mciSendString("open \"Elevator Music.mp3\" type mpegvideo alias mp3", NULL, 0, NULL);
	mciSendString("play mp3 repeat", NULL, 0, NULL);
#endif

	while (processCameraMovement(window) != GLFW_PRESS && !glfwWindowShouldClose(window)) {
		shader.Bind();
		shapeArrSize = shapeArray.GetSize();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		x += l;
		if (x == 150.0f || x == 0.0f) {
			l = (-1.0f)*l;
			//std::cout << "change" << std::endl;
		}
		MVP = Projection * View * Model;

		for (int i = shapeArrSize-1; i >= 0; i--) {
			float* color = shapeArray.GetColor(i);
			ib_size = shapeArray.GetIndexPointerSize(i);
			shapeArray.BindShape(i);
			if (i > 1) {
				shapeArray.Move(i);
			}
			Model = shapeArray.GetModel(i);
			MVP = Projection * View * Model;
			shader.SetUniformMat4f("u_MVP", MVP);
			shader.SetUniform4f("u_Color",color);
			shader.SetUniformMat4f("model", Model);
			shader.SetUniform3f("u_Light", 150.0f, x, 150.0f);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			shader.SetUniform3f("u_vPos", cameraPos.x, cameraPos.y, cameraPos.z);
			if (i == 2) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			}
			if (i == 1&&tex) {

				shader.SetUniform1i("isTexture",2);
				glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
				glDrawElements(GL_TRIANGLES, ib_size, GL_UNSIGNED_INT, nullptr);
				glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
			}
			else {
				shader.SetUniform1i("isTexture", 1);
				glDrawElements(GL_TRIANGLES, ib_size, GL_UNSIGNED_INT, nullptr);
			}
			
			//glBindVertexArray(0); //it works without this*/
		}

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
		shader.Unbind();
	}
	glfwTerminate();

	return 0;
}