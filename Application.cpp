#include "DynamicShapeArray.h"
#include "Shader.h"
#include <glm/gtc/matrix_transform.hpp>

#ifdef _WIN32
	#include <Windows.h>
#endif
#define STB_IMAGE_IMPLEMENTATION   
#include "stb_image.h"

DynamicShapeArray shapeArray;
//camera 
glm::vec3 cameraPos(-200.0f, 200.0f, 200.0f);
glm::vec3 cameraFront(2.0f, -1.2f, -1.2f);
glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);
glm::mat4 View = glm::lookAt(
	cameraPos, // camera position
	(cameraPos + cameraFront), // pos that camera looks at
	cameraUp  // up direction
);

//speeds
float cameraSpeed = 20.0f / GLOBAL_SPEED;
float yawSpeed = 1.0f / GLOBAL_SPEED;

//flags
bool joystick_space = false;
bool joystick_tex = false;
bool joystick_mute = false;

bool spaceChecker = true;
bool texChecker = true;
bool muteChecker = true;

bool tex = true;


//loads texture from file
unsigned int loadTexture()
{
	unsigned int texture;
	glGenTextures(1, &texture);

	//generating cube map
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

	//Define all 6 faces
	// load and generate the texture
	int width, height, nrChannels;
	unsigned char* data = stbi_load("texture.jpg", &width, &height, &nrChannels, STBI_rgb_alpha);
	if (data)
	{
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
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


//process inputs
int processCameraMovement(GLFWwindow* window) {
	int present = glfwJoystickPresent(GLFW_JOYSTICK_1);
	if (1 == present)
	{
		int axesCount;
		const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axesCount);

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

		if (axes[5] >= -1.0)//R2
			cameraPos += (axes[5] + 1) * cameraFront;
		if (abs(axes[1]) >= 0.2)//LY
			cameraPos -= (axes[1]) * cameraUp;
		if (axes[4] >= -1.0)//L2
			cameraPos -= (axes[4] + 1) * cameraFront;
		if (abs(axes[0]) >= 0.2)//RX
			cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * (axes[0]);
		if (abs(axes[3]) >= 0.2)//RY
			cameraFront -= (axes[3] / 30) * cameraUp;
		if (abs(axes[2]) >= 0.2)//RX
			cameraFront += glm::normalize(glm::cross(cameraFront, cameraUp)) * (axes[2] / 30);
		if (buttons[11] == GLFW_PRESS)//right arrow
			shapeArray.MoveSphere(1, glm::vec3(1.0f, 0.0f, 0.0f));
		if (buttons[13] == GLFW_PRESS)//left arrow
			shapeArray.MoveSphere(1, glm::vec3(-1.0f, 0.0f, 0.0f));
		if (buttons[10] == GLFW_PRESS)//up arrow
			shapeArray.MoveSphere(1, glm::vec3(0.0f, 1.0f, 0.0f));
		if (buttons[12] == GLFW_PRESS)//down arrow
			shapeArray.MoveSphere(1, glm::vec3(0.0f, -1.0f, 0.0f));
		if (buttons[4] == GLFW_PRESS)//L1 arrow
			shapeArray.MoveSphere(1, glm::vec3(0.0f, 0.0f, 1.0f));
		if (buttons[5] == GLFW_PRESS)//R1 arrow
			shapeArray.MoveSphere(1, glm::vec3(0.0f, 0.0f, -1.0f));
		if (buttons[6] == GLFW_PRESS)//Select
			shapeArray.SpeedUP(false);
		if (buttons[7] == GLFW_PRESS)//Start
			shapeArray.SpeedUP(true);
		if (buttons[2] == GLFW_PRESS && texChecker) {//X
			texChecker = false;
			joystick_tex = true;
			tex = !tex;
		}
		else if (buttons[2] == GLFW_RELEASE) {
			joystick_tex = false;
			texChecker = true;
		}

		//stop bounce sound
		if (buttons[3] == GLFW_PRESS && muteChecker) {
			muteChecker = false;
			soundsEnabled = !soundsEnabled;
			joystick_mute = true;
		}
		else if (buttons[3] == GLFW_RELEASE && joystick_mute) {
			muteChecker = true;
			joystick_mute = false;
		}
	}


	if ((glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) && spaceChecker && !joystick_space) {
		spaceChecker = false;
		shapeArray.CreateRandomShape();
	}
	else if ((glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) && !joystick_space) {
		spaceChecker = true;
	}
	if ((glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) && texChecker && !joystick_tex) {
		texChecker = false;
		tex = !tex;		
	}
	else if ((glfwGetKey(window, GLFW_KEY_T) == GLFW_RELEASE) && !joystick_tex) {
		texChecker = true;
	}

	//move camera
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

	//camera controls
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

	//speed UP/DOWN
	if ((glfwGetKey(window, GLFW_KEY_PERIOD) == GLFW_PRESS)) {
		shapeArray.SpeedUP(true);
	}
	if ((glfwGetKey(window, GLFW_KEY_COMMA) == GLFW_PRESS)) {
		shapeArray.SpeedUP(false);
	}

	//stop bounce sound
	if ((glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) && muteChecker && !joystick_mute) {
		muteChecker = false;
		soundsEnabled = !soundsEnabled;
	}
	else if ((glfwGetKey(window, GLFW_KEY_M) == GLFW_RELEASE) && !joystick_mute) {
		muteChecker = true;
	}

	View = glm::lookAt( cameraPos, (cameraPos + cameraFront), cameraUp );

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
	glm::mat4 Projection = glm::perspective(glm::radians(40.0f),1.0f,0.001f,1000.0f);



	// Model Matrix
	glm::mat4 Model = glm::mat4(1.0f);

	//ModelViewProjection Matrix
	glm::mat4 MVP;// Projection * View * Model;

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_BLEND);
	glEnable(GL_NORMALIZE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//for the textures
	glDisable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	
	//Create the first 2 shapes
	shapeArray.CreateShape(0.0f, 0.0f, 0.0f, 100.0f, T_CUBE);
	shapeArray.SetRandomColor(0, 0.5f);//give random color to cube
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
#ifdef _WIN32
		if(soundsEnabled)
			mciSendString("resume mp3 ", NULL, 0, NULL);
		else
			mciSendString("pause mp3 ", NULL, 0, NULL);
#endif
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