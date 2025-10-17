#include "ApplicationController.h"
#include <iostream>


ApplicationController::ApplicationController() {
	window = nullptr;
	renderer = nullptr;
	camera = new CameraController();
	shapeArray = new DynamicShapeArray();
	inputController = new InputController(camera, shapeArray);
}
ApplicationController::~ApplicationController() {
	delete camera;
	delete shapeArray;
	delete inputController;
	delete renderer;
}

int ApplicationController::start() {
	
	uint32_t one = 1;
	uint32_t zero = 0;
	renderer = new OpenGLRenderer();
	if (renderer->init(1000, 1000) != 1) return -1; // TODO: Update to error codes
	window = renderer->getWindow();
	if (!window) return -1;
	shapeArray->setRenderer(renderer);
	
	glm::mat4 Projection = glm::perspective(glm::radians(40.0f), 1.0f, 0.1f, 1000.0f);

	// Model Matrix
	glm::mat4 Model = glm::mat4(1.0f);
	glm::mat4 normalModel = glm::mat4(1.0f);


	//ModelViewProjection Matrix
	glm::mat4 MVP;// Projection * View * Model;

	shapeArray->InitFactoryPrototypes();
	renderer->createUBO(0, MODEL_MATRIX, 2 * sizeof(glm::mat4) + sizeof(glm::mat3));
	renderer->createUBO(1, LIGHT_DATA, 2 * sizeof(glm::vec4));
	renderer->createUBO(2, CAMERA_POS, sizeof(glm::vec4));
	renderer->createUBO(3, IS_TEXTURE, 1 * sizeof(uint32_t));


	//Create the first 2 shapes
	shapeArray->CreateShape(0.0f, 0.0f, 0.0f, 100.0f, T_CUBE);
	shapeArray->SetRandomColor(0, 0.5f);//give random color to cube
	shapeArray->CreateShape(35.0f, 35.0f, 35.0f, 30.0f, T_SPHERE);
	shapeArray->SetColor(1, 1.0f, 1.0f, 1.0f, 1.0f);

	shapeArray->CreateRandomShapes(1000);

	//Initialize Shader
	renderer->initShader("Shader.shader");

	unsigned int ib_size;
	uint32_t shapeArrSize;
	float x = 1.0f;
	float l = 1.0f;
	glm::vec3 lightPos{ 150.f, x, 150.f };
	renderer->loadTexture("textures/texture.jpg");
#ifdef _WIN32
	mciSendString("open \"Elevator Music.mp3\" type mpegvideo alias mp3", NULL, 0, NULL);
	mciSendString("play mp3 repeat", NULL, 0, NULL);
#endif

	while (inputController->parseInputs(window) != GLFW_PRESS && !glfwWindowShouldClose(window)) {
#ifdef _WIN32
		if (soundsEnabled)
			mciSendString("resume mp3 ", NULL, 0, NULL);
		else
			mciSendString("pause mp3 ", NULL, 0, NULL);
#endif
		shapeArrSize = shapeArray->getSize();
		renderer->beginFrame();

		x += l;
		lightPos.y = x;
		if (x >= 150.0f || x <= 0.0f) {
			l = (-1.0f) * l;
		}

		// This can be a compute shader, then batch draw (utilize instancing and ssbos).
		// Example:
		// moveShader.bind()
		// glDispatchCompute(shapeArrSize, 1, 1);
		for (int i = shapeArrSize - 1; i >= 0; i--) {
			float* color = shapeArray->GetColor(i);
			ib_size = shapeArray->GetIndexPointerSize(i);
			shapeArray->BindShape(i);
			if (i > 1) {
				shapeArray->Move(i);
			}
			Model = shapeArray->getModel(i);
			normalModel = shapeArray->getNormalModel(i);
			renderer->uploadUBOData(0, MODEL_MATRIX, sizeof(glm::mat4), sizeof(glm::mat4), &Model[0]); // swap with glm::valueptr
			renderer->uploadUBOData(0, MODEL_MATRIX, sizeof(glm::mat3), 2 * sizeof(glm::mat4), &normalModel[0]);
			MVP = Projection * camera->getView() * Model;
			renderer->uploadUBOData(0, MODEL_MATRIX, sizeof(glm::mat4), 0, &MVP[0]);

			// update this to SSBOs
			renderer->uploadUBOData(1, LIGHT_DATA, sizeof(glm::vec4), sizeof(glm::vec4), &color[0]);
			renderer->uploadUBOData(1, LIGHT_DATA, sizeof(glm::vec3), 0, &lightPos[0]);
			renderer->uploadUBOData(2, CAMERA_POS, sizeof(glm::vec3), 0, &camera->getPosition());

			if (i == 1 && tex) {
				renderer->uploadUBOData(3, IS_TEXTURE, sizeof(uint32_t), 0, &zero);
			}
			else {
				renderer->uploadUBOData(3, IS_TEXTURE, sizeof(uint32_t), 0, &one);
			}
			renderer->drawElements(ib_size);
		}

		renderer->endFrame();
	}
	return 0;
}
