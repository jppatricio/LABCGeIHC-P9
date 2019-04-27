//glew include
#include <GL/glew.h>

//std includes
#include <string>
#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>

//glfw include
#include <GLFW/glfw3.h>

//GLM include
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// program include
#include "Headers/TimeManager.h"
#include "Headers/Shader.h"
// Geometry primitives
#include "Headers/Sphere.h"
#include "Headers/Cylinder.h"
#include "Headers/Box.h"
#include "Headers/FirstPersonCamera.h"
//Texture includes
#include "Headers/Texture.h"

#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))

std::shared_ptr<FirstPersonCamera> camera(new FirstPersonCamera());

Sphere sphere(20, 20);
Sphere sphere2(15, 15);
Cylinder cylinder(20, 20, 0.5, 0.5);
Box box;
Box mesaTop, mesaPataUno, mesaPataDos, sillon;
Box mainPlane, windowBox;

Shader shaderColor;
Shader shaderColor2;
Shader shaderTexture;
Shader shaderCubeTexture;
Shader shaderMateriales;
Shader shaderDirectionLight;
Shader shaderPointLight;
Shader shaderPointLight2;
Shader shaderSpotLight;

bool lightON = true;

GLuint textureID1, textureID2, textureID3, textureCubeTexture, textureIDWhiteWall, textureIDCap;
GLuint cubeTextureID, textureIDMaderaMarco, textureIDWindowFrame, textureIDpiso, textureIDMesa, textureIDSillonObs, textureIDSillonCla;

GLenum types[6] = {
	GL_TEXTURE_CUBE_MAP_POSITIVE_X,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
};

std::string fileNames[6] = { // SE CAMBIAN A LAS NUEVAS TEXTURAS PARA EL SKYBOX
	"../../Textures/ely_hills/hills_ft.tga",
	"../../Textures/ely_hills/hills_bk.tga",
	"../../Textures/ely_hills/hills_up.tga",
	"../../Textures/ely_hills/hills_dn.tga",
	"../../Textures/ely_hills/hills_rt.tga",
	"../../Textures/ely_hills/hills_lf.tga"
};

int screenWidth;
int screenHeight;

GLFWwindow * window;

bool exitApp = false;
int lastMousePosX, offsetX;
int lastMousePosY, offsetY;

double deltaTime;

// Se definen todos las funciones.
void reshapeCallback(GLFWwindow* Window, int widthRes, int heightRes);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void mouseButtonCallback(GLFWwindow* window, int button, int state, int mod);
void init(int width, int height, std::string strTitle, bool bFullScreen);
void destroyWindow();
void destroy();
bool processInput(bool continueApplication = true);

// Implementacion de todas las funciones.
void init(int width, int height, std::string strTitle, bool bFullScreen) {

	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW" << std::endl;
		exit(-1);
	}

	screenWidth = width;
	screenHeight = height;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	if (bFullScreen)
		window = glfwCreateWindow(width, height, strTitle.c_str(),
			glfwGetPrimaryMonitor(), nullptr);
	else
		window = glfwCreateWindow(width, height, strTitle.c_str(), nullptr,
			nullptr);

	if (window == nullptr) {
		std::cerr
			<< "Error to create GLFW window, you can try download the last version of your video card that support OpenGL 3.3+"
			<< std::endl;
		destroyWindow();
		exit(-1);
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(0);

	glfwSetWindowSizeCallback(window, reshapeCallback);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Init glew
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		std::cerr << "Failed to initialize glew" << std::endl;
			exit(-1);
	}

	glViewport(0, 0, screenWidth, screenHeight);
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	glEnable(GL_DEPTH_TEST);

	shaderColor.initialize("../../Shaders/transformaciones.vs", "../../Shaders/transformaciones.fs");
	shaderColor2.initialize("../../Shaders/transformaciones.vs", "../../Shaders/transformaciones.fs");
	shaderTexture.initialize("../../Shaders/texturizado_res.vs", "../../Shaders/texturizado_res.fs");
	shaderCubeTexture.initialize("../../Shaders/cubeTexture.vs", "../../Shaders/cubeTexture.fs");
	shaderMateriales.initialize("../../Shaders/iluminacion_materiales_res.vs", "../../Shaders/iluminacion_materiales_res.fs");
	shaderDirectionLight.initialize("../../Shaders/typeLight.vs", "../../Shaders/directionalLight.fs");
	shaderPointLight.initialize("../../Shaders/typeLight.vs", "../../Shaders/pointLight.fs");
	shaderPointLight2.initialize("../../Shaders/typeLight.vs", "../../Shaders/pointLight.fs");
	shaderSpotLight.initialize("../../Shaders/typeLight.vs", "../../Shaders/spotLight.fs");

	sphere.init();
	sphere2.init();
	cylinder.init();
	box.init();
	mainPlane.init();
	windowBox.init();
	mesaTop.init();
	sillon.init();

	camera->setPosition(glm::vec3(0.0f, 0.7f, 0.4f));

	// Textura Ladrillos
	int imageWidth, imageHeight;
	Texture texture("../../Textures/texturaLadrillos.jpg");
	FIBITMAP* bitmap = texture.loadImage(false);
	unsigned char * data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureID1);
	glBindTexture(GL_TEXTURE_2D, textureID1);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data){
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture.freeImage(bitmap);

	//textureIDMaderaMarco
	imageWidth, imageHeight;
	texture = Texture("../../Textures/woodenDoor.png");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureIDMaderaMarco);
	glBindTexture(GL_TEXTURE_2D, textureIDMaderaMarco);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture.freeImage(bitmap); 

	//textureIDpiso
	imageWidth, imageHeight;
	texture = Texture("../../Textures/piso.png");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureIDpiso);
	glBindTexture(GL_TEXTURE_2D, textureIDpiso);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture.freeImage(bitmap);

	//textureIDMesa
	imageWidth, imageHeight;
	texture = Texture("../../Textures/mesaMadera.png");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureIDMesa);
	glBindTexture(GL_TEXTURE_2D, textureIDMesa);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture.freeImage(bitmap);

	//textureIDSillonObs
	imageWidth, imageHeight;
	texture = Texture("../../Textures/sillonObscuro.png");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureIDSillonObs);
	glBindTexture(GL_TEXTURE_2D, textureIDSillonObs);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture.freeImage(bitmap);

	//textureIDSillonCla
	imageWidth, imageHeight;
	texture = Texture("../../Textures/sillonClaro.png");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureIDSillonCla);
	glBindTexture(GL_TEXTURE_2D, textureIDSillonCla);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture.freeImage(bitmap);
	//textureIDWindowFrame
	imageWidth, imageHeight;
	texture = Texture("../../Textures/windowFrame.png");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureIDWindowFrame);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBindTexture(GL_TEXTURE_2D, textureIDWindowFrame);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture.freeImage(bitmap);

	// Texture GRASS
	texture = Texture("../../Textures/grass.jpg");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureID2);
	glBindTexture(GL_TEXTURE_2D, textureID2);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture.freeImage(bitmap);

	// Texture WHITE WALL
	texture = Texture("../../Textures/white_wall.jpg");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureIDWhiteWall);
	glBindTexture(GL_TEXTURE_2D, textureIDWhiteWall);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture.freeImage(bitmap);

	// Textura cuadritos
	texture = Texture("../../Textures/test.png");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureID3);
	glBindTexture(GL_TEXTURE_2D, textureID3);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture.freeImage(bitmap);




	// Textura metal
	texture = Texture("../../Textures/bottomCap.png");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureIDCap);
	glBindTexture(GL_TEXTURE_2D, textureIDCap);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture.freeImage(bitmap);

	// en vex de gl_texture_2d es cube_map para poder hacer el skybox
	glGenTextures(1, &cubeTextureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTextureID);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	for (int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(types); i++) {
		texture = Texture(fileNames[i]);
		FIBITMAP* bitmap = texture.loadImage(true);
		data = texture.convertToData(bitmap, imageWidth, imageHeight);
		if (data) {
			glTexImage2D(types[i], 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		}
		else
			std::cout << "Failed to load texture" << std::endl;
		texture.freeImage(bitmap);
	}

}

void destroyWindow() {
	glfwDestroyWindow(window);
	glfwTerminate();
}

void destroy() {
	destroyWindow();

	shaderColor.destroy();
	shaderTexture.destroy();
	shaderCubeTexture.destroy();
	shaderMateriales.destroy();
	shaderDirectionLight.destroy();
	shaderPointLight.destroy();
	shaderSpotLight.destroy();
	sphere.destroy();
	cylinder.destroy();
	box.destroy();
}

void reshapeCallback(GLFWwindow* Window, int widthRes, int heightRes) {
	screenWidth = widthRes;
	screenHeight = heightRes;
	glViewport(0, 0, widthRes, heightRes);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_ESCAPE:
			exitApp = true;
			break;
		}
	}
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
	offsetX = xpos - lastMousePosX;
	//offsetY = ypos - lastMousePosY;
	lastMousePosX = xpos;
	//lastMousePosY = ypos;
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		camera->mouseMoveCamera(offsetX, 0.0, deltaTime);
}

void mouseButtonCallback(GLFWwindow* window, int button, int state, int mod) {
	if (state == GLFW_PRESS) {
		switch (button) {
		case GLFW_MOUSE_BUTTON_RIGHT:
			std::cout << "lastMousePos.y:" << lastMousePosY << std::endl;
			break;
		case GLFW_MOUSE_BUTTON_LEFT:
			std::cout << "lastMousePos.x:" << lastMousePosX << std::endl;
			break;
		case GLFW_MOUSE_BUTTON_MIDDLE:
			std::cout << "lastMousePos.x:" << lastMousePosX << std::endl;
			std::cout << "lastMousePos.y:" << lastMousePosY << std::endl;
			break;
		}
	}
}
int timePassed = 0;
bool processInput(bool continueApplication) {
	if (exitApp || glfwWindowShouldClose(window) != 0) {
		return false;
	}
	if (timePassed > 0) {
		timePassed = timePassed - 1;
	}
	TimeManager::Instance().CalculateFrameRate(false);
	deltaTime = TimeManager::Instance().DeltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera->moveFrontCamera(true, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera->moveFrontCamera(false, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera->moveRightCamera(false, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera->moveRightCamera(true, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
		if (timePassed == 0) {
			lightON = !lightON;
			timePassed = 10;
		}
	}
	glfwPollEvents();
	return continueApplication;
}

void applicationLoop() {
	bool psi = true;
	double lastTime = TimeManager::Instance().GetTime();

	glm::vec3 objPosition = glm::vec3(0.0f, 0.0f, -3.0f);
	float angle = 0.0;
	float ratio = 5.0;

	while (psi) {
		psi = processInput(true);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.2f, 0.3f, 0.3f, 0.0f);

		// Matrix de proyeccion en perspectiva
		glm::mat4 projection = glm::perspective(glm::radians(45.0f),
			(float)screenWidth / screenWidth, 0.01f, 100.0f);
		// matrix de vista
		glm::mat4 view = camera->getViewMatrix();

		glm::mat4 cubeModelMatrix = glm::translate(glm::mat4(1.0f), objPosition);

		//--------------------------MAIN PLANE
		for (float x = -10; x < 10; x = x + 0.5) {
			for (float y = -10; y < 10; y = y + 0.5) {
				if (x <= 0.5 && x >= -5.5 && y >= -5 && y <= 1.0) {
					glBindTexture(GL_TEXTURE_2D, textureIDpiso);
				}
				else {
					glBindTexture(GL_TEXTURE_2D, textureID2);
				}
				mainPlane.setProjectionMatrix(projection);
				mainPlane.setViewMatrix(view);
				mainPlane.setShader(&shaderPointLight);
				mainPlane.setScale(glm::vec3(0.5, 0.01, 0.5));
				mainPlane.setPosition(glm::vec3(x, 0.01f, y));
				mainPlane.render();
			}
		}
		for (float x = -5.5; x <= 0.5; x = x + 0.5) {
			for (float y = -5; y <= 1; y = y + 0.5) {
				glBindTexture(GL_TEXTURE_2D, textureIDWhiteWall);
				mainPlane.setProjectionMatrix(projection);
				mainPlane.setViewMatrix(view);
				mainPlane.setShader(&shaderPointLight);
				mainPlane.setScale(glm::vec3(0.5, 0.01, 0.5));
				mainPlane.setPosition(glm::vec3(x, 1.0f, y));
				mainPlane.render();

				glBindTexture(GL_TEXTURE_2D, textureIDWhiteWall);
				mainPlane.setProjectionMatrix(projection);
				mainPlane.setViewMatrix(view);
				mainPlane.setShader(&shaderColor);
				mainPlane.setScale(glm::vec3(0.5, 0.01, 0.5));
				mainPlane.setPosition(glm::vec3(x, 2.0f, y));
				mainPlane.render();
			}
		}
		//--------------------------------------

		//----- Mesa
		glBindTexture(GL_TEXTURE_2D, textureIDMesa);
		mesaTop.setProjectionMatrix(projection);
		mesaTop.setViewMatrix(view);
		mesaTop.setShader(&shaderPointLight);

		mesaTop.setScale(glm::vec3(1.0, 0.05, 1.0));
		mesaTop.setPosition(glm::vec3(-1.5, 0.1, -3.0));
		mesaTop.render();

		glBindTexture(GL_TEXTURE_2D, textureIDSillonObs);
		mesaTop.setProjectionMatrix(projection);
		mesaTop.setViewMatrix(view);
		mesaTop.setShader(&shaderPointLight);

		mesaTop.setScale(glm::vec3(0.8, 0.1, 0.8));
		mesaTop.setPosition(glm::vec3(-1.5, 0.0, -3.0));
		mesaTop.render();
		//--------------------------

		//----- Sillones
		glBindTexture(GL_TEXTURE_2D, textureIDSillonCla);
		sillon.setProjectionMatrix(projection);
		sillon.setViewMatrix(view);
		sillon.setShader(&shaderPointLight);

		sillon.setScale(glm::vec3(0.49, 0.05, 0.4));
		sillon.setPosition(glm::vec3(-2.0, 0.2, -4.0));
		sillon.render();
		sillon.setPosition(glm::vec3(-1.5, 0.2, -4.0));
		sillon.render();
		sillon.setPosition(glm::vec3(-1., 0.2, -4.0));
		sillon.render();

		sillon.setScale(glm::vec3(0.48, 0.3, 0.06));
		sillon.setPosition(glm::vec3(-2.0, 0.2, -4.11));
		sillon.render();
		sillon.setPosition(glm::vec3(-1.5, 0.2, -4.11));
		sillon.render();
		sillon.setPosition(glm::vec3(-1., 0.2, -4.11));
		sillon.render();



		glBindTexture(GL_TEXTURE_2D, textureIDSillonObs);
		sillon.setProjectionMatrix(projection);
		sillon.setViewMatrix(view);
		sillon.setShader(&shaderPointLight);

		sillon.setScale(glm::vec3(1.6, 0.2, 0.4));
		sillon.setPosition(glm::vec3(-1.5, 0.1, -4.0));
		sillon.render();

		sillon.setScale(glm::vec3(1.6, 0.45, 0.1));
		sillon.setPosition(glm::vec3(-1.5, 0.18, -4.2));
		sillon.render();

		sillon.setScale(glm::vec3(0.1, 0.15, 0.4));
		sillon.setPosition(glm::vec3(-2.25, 0.2, -4.0));
		sillon.render();
		sillon.setPosition(glm::vec3(-0.75, 0.2, -4.0));
		sillon.render();

		// S2------------------------vvvvvvvvvvvvvvvvvvv
		glBindTexture(GL_TEXTURE_2D, textureIDSillonCla);
		sillon.setProjectionMatrix(projection);
		sillon.setViewMatrix(view);
		sillon.setShader(&shaderPointLight);

		sillon.setScale(glm::vec3(0.49, 0.05, 0.4));
		sillon.setPosition(glm::vec3(-0.5, 0.2, -3.0));
		sillon.render();

		sillon.setScale(glm::vec3(0.06, 0.3, 0.48));
		sillon.setPosition(glm::vec3(-0.36, 0.2, -3.0));
		sillon.render();

		glBindTexture(GL_TEXTURE_2D, textureIDSillonObs);
		sillon.setProjectionMatrix(projection);
		sillon.setViewMatrix(view);
		sillon.setShader(&shaderPointLight);

		sillon.setScale(glm::vec3(0.4, 0.2, 0.53));
		sillon.setPosition(glm::vec3(-0.55, 0.1, -3.0));
		sillon.render();

		sillon.setScale(glm::vec3(0.1, 0.45, 0.53));
		sillon.setPosition(glm::vec3(-0.3, 0.18, -3.0));
		sillon.render();

		sillon.setScale(glm::vec3(0.4, 0.15, 0.1));
		sillon.setPosition(glm::vec3(-0.5, 0.2, -3.25));
		sillon.render();
		sillon.setPosition(glm::vec3(-0.5, 0.2, -2.75));
		sillon.render();
		//--wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww
		//--------------------------

		//----------------------------MAIN WALLS
		glBindTexture(GL_TEXTURE_2D, textureIDWhiteWall);
		mainPlane.setProjectionMatrix(projection);
		mainPlane.setViewMatrix(view);
		mainPlane.setShader(&shaderPointLight);

		mainPlane.setScale(glm::vec3(1.0, 1.0, 0.01));
		mainPlane.setPosition(glm::vec3(-5, 0.51f, -5));
		mainPlane.render();
		mainPlane.setPosition(glm::vec3(-4, 0.51f, -5));
		mainPlane.render();
		mainPlane.setPosition(glm::vec3(-3, 0.51f, -5));
		mainPlane.render();



		mainPlane.setShader(&shaderPointLight);

		glBindTexture(GL_TEXTURE_2D, textureIDWhiteWall);
		mainPlane.setScale(glm::vec3(1.0, 1.0, 0.01));
		mainPlane.setPosition(glm::vec3(0, 0.51f, -5));
		mainPlane.render();

		mainPlane.setScale(glm::vec3(0.01, 1.0, 1.0));
		mainPlane.setPosition(glm::vec3(-5.5, 0.51f, -4.5));
		mainPlane.render();
		mainPlane.setPosition(glm::vec3(-5.5, 0.51f, -3.5));
		mainPlane.render();
		mainPlane.setPosition(glm::vec3(-5.5, 0.51f, -2.5));
		mainPlane.render();
		mainPlane.setPosition(glm::vec3(-5.5, 0.51f, -1.5));
		mainPlane.render();
		mainPlane.setPosition(glm::vec3(-5.5, 0.51f, -0.5));
		mainPlane.render();
		mainPlane.setPosition(glm::vec3(-5.5, 0.51f, 0.5));
		mainPlane.render();

		mainPlane.setScale(glm::vec3(0.01, 1.0, 1.0));
		mainPlane.setPosition(glm::vec3(.5, 0.51f, -4.5));
		mainPlane.render();
		mainPlane.setPosition(glm::vec3(.5, 0.51f, -3.5));
		mainPlane.render();
		mainPlane.setPosition(glm::vec3(.5, 0.51f, -2.5));
		mainPlane.render();
		mainPlane.setPosition(glm::vec3(.5, 0.51f, -1.5));
		mainPlane.render();
		mainPlane.setPosition(glm::vec3(.5, 0.51f, -0.5));
		mainPlane.render();
		mainPlane.setPosition(glm::vec3(.5, 0.51f, 0.5));
		mainPlane.render();

		mainPlane.setScale(glm::vec3(1.0, 1.0, 0.01));
		mainPlane.setPosition(glm::vec3(-5, 0.51f, 1.0));
		mainPlane.render();
		mainPlane.setPosition(glm::vec3(-4, 0.51f, 1.0));
		mainPlane.render();
		mainPlane.setPosition(glm::vec3(-3, 0.51f, 1.0));
		mainPlane.render();
		mainPlane.setPosition(glm::vec3(-2, 0.51f, 1.0));
		mainPlane.render();
		mainPlane.setPosition(glm::vec3(-1, 0.51f, 1.0));
		mainPlane.render();
		mainPlane.setPosition(glm::vec3(0, 0.51f, 1.0));
		mainPlane.render();

		// Puerta Principal
		glBindTexture(GL_TEXTURE_2D, textureIDMaderaMarco);
		mainPlane.setScale(glm::vec3(0.6, 0.95, 0.02));
		mainPlane.setPosition(glm::vec3(-5, 0.51f, -5));
		mainPlane.render();
		//--------------------------------------

		//----------------------------WALLALALALLA
		glBindTexture(GL_TEXTURE_2D, textureIDWhiteWall);
		mainPlane.setProjectionMatrix(projection);
		mainPlane.setViewMatrix(view);
		mainPlane.setShader(&shaderPointLight);

		mainPlane.setScale(glm::vec3(1.0, 1.0, 0.01));
		mainPlane.setPosition(glm::vec3(-5, 0.51f, -5));
		mainPlane.render();
		mainPlane.setPosition(glm::vec3(-4, 0.51f, -5));
		mainPlane.render();
		mainPlane.setPosition(glm::vec3(-3, 0.51f, -5));
		mainPlane.render();

		//----------------------------MAIN WALLS
		glBindTexture(GL_TEXTURE_2D, textureIDWhiteWall);
		mainPlane.setProjectionMatrix(projection);
		mainPlane.setViewMatrix(view);
		mainPlane.setShader(&shaderPointLight);

		mainPlane.setScale(glm::vec3(1.0, 1.0, 0.01));
		mainPlane.setPosition(glm::vec3(-5, 1.51f, -5));
		mainPlane.render();
		mainPlane.setPosition(glm::vec3(-4, 1.51f, -5));
		mainPlane.render();
		mainPlane.setPosition(glm::vec3(-3, 1.51f, -5));
		mainPlane.render();



		mainPlane.setShader(&shaderPointLight);

		glBindTexture(GL_TEXTURE_2D, textureIDWhiteWall);
		mainPlane.setScale(glm::vec3(1.0, 1.0, 0.01));
		mainPlane.setPosition(glm::vec3(0, 1.51f, -5));
		mainPlane.render();

		mainPlane.setScale(glm::vec3(0.01, 1.0, 1.0));
		mainPlane.setPosition(glm::vec3(-5.5, 1.51f, -4.5));
		mainPlane.render();
		mainPlane.setPosition(glm::vec3(-5.5, 1.51f, -3.5));
		mainPlane.render();
		mainPlane.setPosition(glm::vec3(-5.5, 1.51f, -2.5));
		mainPlane.render();
		mainPlane.setPosition(glm::vec3(-5.5, 1.51f, -1.5));
		mainPlane.render();
		mainPlane.setPosition(glm::vec3(-5.5, 1.51f, -0.5));
		mainPlane.render();
		mainPlane.setPosition(glm::vec3(-5.5, 1.51f, 0.5));
		mainPlane.render();

		mainPlane.setScale(glm::vec3(0.01, 1.0, 1.0));
		mainPlane.setPosition(glm::vec3(.5, 1.51f, -4.5));
		mainPlane.render();
		mainPlane.setPosition(glm::vec3(.5, 1.51f, -3.5));
		mainPlane.render();
		mainPlane.setPosition(glm::vec3(.5, 1.51f, -2.5));
		mainPlane.render();
		mainPlane.setPosition(glm::vec3(.5, 1.51f, -1.5));
		mainPlane.render();
		mainPlane.setPosition(glm::vec3(.5, 1.51f, -0.5));
		mainPlane.render();
		mainPlane.setPosition(glm::vec3(.5, 1.51f, 0.5));
		mainPlane.render();

		mainPlane.setScale(glm::vec3(1.0, 1.0, 0.01));
		mainPlane.setPosition(glm::vec3(-5, 1.51f, 1.0));
		mainPlane.render();
		mainPlane.setPosition(glm::vec3(-4, 1.51f, 1.0));
		mainPlane.render();
		mainPlane.setPosition(glm::vec3(-3, 1.51f, 1.0));
		mainPlane.render();
		mainPlane.setPosition(glm::vec3(-2, 1.51f, 1.0));
		mainPlane.render();
		mainPlane.setPosition(glm::vec3(-1, 1.51f, 1.0));
		mainPlane.render();
		mainPlane.setPosition(glm::vec3(0, 1.51f, 1.0));
		mainPlane.render();


		// shaderMateriales -> shaderDirectionLight
		// shaderDirectionLight -> shaderPointLight
		//shaderSpotLight
		//--

		/*sphere2.setShader(&shaderPointLight);
		sphere2.setProjectionMatrix(projection);
		sphere2.setViewMatrix(view);
		sphere2.setPosition(glm::vec3(0.0, 0.0, 0.0));
		sphere2.setScale(glm::vec3(1.0, 1.0, 1.0));*/

		shaderSpotLight.turnOn();
		glUniform3fv(shaderSpotLight.getUniformLocation("light.position"), 1, glm::value_ptr(camera->getPosition()));
		glUniform3fv(shaderSpotLight.getUniformLocation("light.direction"), 1, glm::value_ptr(camera->getFront()));
		glUniform3fv(shaderSpotLight.getUniformLocation("viewPos"), 1, glm::value_ptr(camera->getPosition()));
		glUniform3f(shaderSpotLight.getUniformLocation("light.ambient"), 0.2, 0.2, 0.2);
		glUniform3f(shaderSpotLight.getUniformLocation("light.diffuse"), 0.2, 0.3, 0.6);
		glUniform3f(shaderSpotLight.getUniformLocation("light.specular"), 0.5, 0.3, 0.2);
		glUniform3f(shaderSpotLight.getUniformLocation("material.ambient"), 1.0, 0.2, 0.6);
		glUniform3f(shaderSpotLight.getUniformLocation("material.diffuse"), 0.4, 0.4, 0.4);
		glUniform3f(shaderSpotLight.getUniformLocation("material.specular"), 0.4, 0.4, 0.4);
		//Para el cono----------------------vv
		glUniform1f(shaderSpotLight.getUniformLocation("light.cutOff"), glm::cos(glm::radians(8.5f)));
		glUniform1f(shaderSpotLight.getUniformLocation("light.outerCutOff"), glm::cos(glm::radians(15.0f)));
		//Para el cono----------------------^^
		glUniform1f(shaderSpotLight.getUniformLocation("light.constant"), 1.0f);
		glUniform1f(shaderSpotLight.getUniformLocation("light.linear"), 0.05f);
		glUniform1f(shaderSpotLight.getUniformLocation("light.quadratics"), 1.0f);

		glUniform1f(shaderPointLight.getUniformLocation("material.shininess"), 32.0);
		//sphere2.render();
		shaderSpotLight.turnOff();
		////--
		//sphere2.setShader(&shaderPointLight);
		//sphere2.setProjectionMatrix(projection);
		//sphere2.setViewMatrix(view);
		//sphere2.setPosition(glm::vec3(0.0, 0.0, 0.0));
		//sphere2.setScale(glm::vec3(1.0, 1.0, 1.0));
		// Iluminación
		//glm::mat4 lightModelmatrix = glm::rotate(cubeModelMatrix, angle, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 lightModelmatrix = cubeModelMatrix;
		lightModelmatrix = glm::translate(lightModelmatrix, glm::vec3(-2.0f, 1.0f, 0.0f));
		shaderPointLight.turnOn();
		//glUniform3fv(shaderPointLight.getUniformLocation("light.position"), 1, glm::value_ptr(glm::vec3(lightModelmatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f))));
		glUniform3fv(shaderPointLight.getUniformLocation("light.position"), 1, glm::value_ptr(glm::vec3(lightModelmatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f))));
		//Static Point Light
		//glUniform3fv(shaderPointLight.getUniformLocation("light.position"), 1, glm::value_ptr(glm::vec3((2.0f, 0.0f, 5.0f))));
		glUniform3fv(shaderPointLight.getUniformLocation("light.direction"), 1, glm::value_ptr(glm::vec3(-0.3f, -1.0f, -0.2f)));
		//glUniform3fv(shaderMateriales.getUniformLocation("light.position"), 1, glm::value_ptr(camera->getPosition()));
		//glUniform3fv(shaderMateriales.getUniformLocation("light.direction"),1, glm::value_ptr(camera->getFront()));
		glUniform3fv(shaderPointLight.getUniformLocation("viewPos"), 1, glm::value_ptr(camera->getPosition()));
		glUniform3f(shaderPointLight.getUniformLocation("light.ambient"), 0.2, 0.2, 0.2);
		glUniform3f(shaderPointLight.getUniformLocation("light.diffuse"), 0.2, 0.3, 0.6);
		glUniform3f(shaderPointLight.getUniformLocation("light.specular"), 0.0, 0.3, 0.2);
		glUniform3f(shaderPointLight.getUniformLocation("material.ambient"), 1.0, 0.2, 0.6);
		glUniform3f(shaderPointLight.getUniformLocation("material.diffuse"), 0.4, 0.4, 0.4);
		glUniform3f(shaderPointLight.getUniformLocation("material.specular"), 0.4, 0.4, 0.4);
		glUniform1f(shaderPointLight.getUniformLocation("light.cutOff"), glm::cos(glm::radians(5.0f)));
		glUniform1f(shaderPointLight.getUniformLocation("light.outerCutOff"), glm::cos(glm::radians(15.0f)));

		//para la point light
		if (lightON)
			glUniform1f(shaderPointLight.getUniformLocation("light.constant"), 0.5f);
		else
			glUniform1f(shaderPointLight.getUniformLocation("light.constant"), 100.0f);
		glUniform1f(shaderPointLight.getUniformLocation("light.linear"), 0.5f);
		glUniform1f(shaderPointLight.getUniformLocation("light.quadratics"), 1.0f);

		glUniform1f(shaderPointLight.getUniformLocation("material.shininess"), 2.0);
		//sphere2.render();
		/*cylinder.setPosition(glm::vec3(20.0, 0.0, -20.0));
		cylinder.render();*/

		shaderPointLight.turnOff();

		//-----NEW LIGHT
		//glm::mat4 lightModelmatrix2 = cubeModelMatrix;
		//lightModelmatrix2 = glm::translate(lightModelmatrix2, glm::vec3(-4.0f, 0.0f, 0.0f));
		//shaderPointLight2.turnOn();
		////glUniform3fv(shaderPointLight.getUniformLocation("light.position"), 1, glm::value_ptr(glm::vec3(lightModelmatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f))));
		//glUniform3fv(shaderPointLight2.getUniformLocation("light.position"), 1, glm::value_ptr(glm::vec3(lightModelmatrix2 * glm::vec4(1.0f, 0.0f, 0.0f, 1.0f))));
		////Static Point Light
		////glUniform3fv(shaderPointLight.getUniformLocation("light.position"), 1, glm::value_ptr(glm::vec3((2.0f, 0.0f, 5.0f))));
		//glUniform3fv(shaderPointLight2.getUniformLocation("light.direction"), 1, glm::value_ptr(glm::vec3(-0.3f, -1.0f, -0.2f)));
		////glUniform3fv(shaderMateriales.getUniformLocation("light.position"), 1, glm::value_ptr(camera->getPosition()));
		////glUniform3fv(shaderMateriales.getUniformLocation("light.direction"),1, glm::value_ptr(camera->getFront()));
		//glUniform3fv(shaderPointLight2.getUniformLocation("viewPos"), 1, glm::value_ptr(camera->getPosition()));
		//glUniform3f(shaderPointLight2.getUniformLocation("light.ambient"), 0.2, 0.2, 0.2);
		//glUniform3f(shaderPointLight2.getUniformLocation("light.diffuse"), 0.2, 0.3, 0.6);
		//glUniform3f(shaderPointLight2.getUniformLocation("light.specular"), 0.0, 0.3, 0.2);
		//glUniform3f(shaderPointLight2.getUniformLocation("material.ambient"), 1.0, 0.2, 0.6);
		//glUniform3f(shaderPointLight2.getUniformLocation("material.diffuse"), 0.4, 0.4, 0.4);
		//glUniform3f(shaderPointLight2.getUniformLocation("material.specular"), 0.4, 0.4, 0.4);
		//glUniform1f(shaderPointLight2.getUniformLocation("light.cutOff"), glm::cos(glm::radians(5.0f)));
		//glUniform1f(shaderPointLight2.getUniformLocation("light.outerCutOff"), glm::cos(glm::radians(15.0f)));

		////para la point light
		//glUniform1f(shaderPointLight2.getUniformLocation("light.constant"), 0.3f);
		//glUniform1f(shaderPointLight2.getUniformLocation("light.linear"), 0.5f);
		//glUniform1f(shaderPointLight2.getUniformLocation("light.quadratics"), 1.0f);

		//glUniform1f(shaderPointLight2.getUniformLocation("material.shininess"), 22.0);
		////sphere2.render();
		///*cylinder.setPosition(glm::vec3(20.0, 0.0, -20.0));
		//cylinder.render();*/

		//shaderPointLight2.turnOff();
		//-------------------------------------------------
		glBindTexture(GL_TEXTURE_2D, 0);

		/*if (angle > 2 * M_PI)
			angle = 0.0;
		else
			angle += 0.008;*/

		sphere.setShader(&shaderColor);
		sphere.setColor(glm::vec4(0.29f, 0.14f, 0.0f, 0.2));
		sphere.setProjectionMatrix(projection);
		sphere.setViewMatrix(view);
		sphere.setScale(glm::vec3(0.15f, 0.15f, 0.15f));
		//sphere.enableWireMode();
		sphere.render(lightModelmatrix);

		sphere2.setPosition(glm::vec3(0.0f, 0.0f, -2.0f));

		sphere2.setShader(&shaderColor);
		sphere2.setColor(glm::vec4(0.29f, 0.14f, 0.0f, 0.2));
		sphere2.setProjectionMatrix(projection);
		sphere2.setViewMatrix(view);
		sphere2.setScale(glm::vec3(0.15f, 0.15f, 0.15f));
		//sphere.enableWireMode();
		//sphere2.render(lightModelmatrix2);





		// Se Dibuja el Skybox
		GLint oldCullFaceMode;
		GLint oldDepthFuncMode;
		glGetIntegerv(GL_CULL_FACE_MODE, &oldCullFaceMode);
		glGetIntegerv(GL_DEPTH_FUNC, &oldDepthFuncMode);
		shaderCubeTexture.turnOn();
		GLuint cubeTextureId = shaderCubeTexture.getUniformLocation("skybox");
		glUniform1f(cubeTextureId, 0);
		glCullFace(GL_FRONT);
		glDepthFunc(GL_LEQUAL);
		sphere.setShader(&shaderCubeTexture);
		sphere.setProjectionMatrix(projection);
		//sphere.setViewMatrix(view); // El skybox no se mueve
		sphere.setViewMatrix(glm::mat4(glm::mat3(view))); // El skybox se mueve dependiendo del view
		sphere.setScale(glm::vec3(20.0f, 20.0f, 20.0f));
		sphere.render();
		glCullFace(oldCullFaceMode);
		glDepthFunc(oldDepthFuncMode);
		shaderCubeTexture.turnOff();

		// ventana enfrente
		glBindTexture(GL_TEXTURE_2D, textureIDWhiteWall);
		mainPlane.setShader(&shaderPointLight);
		mainPlane.setScale(glm::vec3(1.0, 0.25, 0.01));
		mainPlane.setPosition(glm::vec3(-2, 0.01f, -5));
		mainPlane.render();
		mainPlane.setPosition(glm::vec3(-1, 0.01f, -5));
		mainPlane.render();
		mainPlane.setScale(glm::vec3(2.0, 0.13, 0.01));
		mainPlane.setPosition(glm::vec3(-1.5, 0.945f, -5));
		mainPlane.render();


		mainPlane.setScale(glm::vec3(0.01, 2.0, 2.0));
		mainPlane.setPosition(glm::vec3(-4.2, 0.0f, -4));
		mainPlane.render();
		mainPlane.setPosition(glm::vec3(-2.8, 0.0f, -4));
		mainPlane.render();

		mainPlane.setScale(glm::vec3(1.4, 2.0, 0.01));
		mainPlane.setPosition(glm::vec3(-3.5, 0.0f, -3));
		mainPlane.render();

	//--//
		mainPlane.setScale(glm::vec3(0.01, 2.0, 2.0));
		mainPlane.setPosition(glm::vec3(-2.8, 0.0f, -0));
		mainPlane.render();

		mainPlane.setScale(glm::vec3(2.8, 2.0, 0.01));
		mainPlane.setPosition(glm::vec3(-4.1, 0.0f, -1.5));
		mainPlane.render();
	//--

		glBindTexture(GL_TEXTURE_2D, textureIDWindowFrame);

		windowBox.setProjectionMatrix(projection);
		windowBox.setViewMatrix(view);
		windowBox.setShader(&shaderTexture);
		windowBox.setScale(glm::vec3(2.0, 0.75, 0.01));
		windowBox.setPosition(glm::vec3(-1.5, 0.5f, -5));
		windowBox.render();
		//--------------------------------------------

		// ventana enfrente 2
		mainPlane.setScale(glm::vec3(1.0, 0.25, 0.01));
		mainPlane.setPosition(glm::vec3(-2, 1.01f, -5));
		mainPlane.render();

		mainPlane.setPosition(glm::vec3(-1, 1.01f, -5));
		mainPlane.render();

		mainPlane.setScale(glm::vec3(2.0, 0.13, 0.01));
		mainPlane.setPosition(glm::vec3(-1.5, 1.945f, -5));
		mainPlane.render();


		glBindTexture(GL_TEXTURE_2D, textureIDWindowFrame);

		windowBox.setProjectionMatrix(projection);
		windowBox.setViewMatrix(view);
		windowBox.setShader(&shaderTexture);
		windowBox.setScale(glm::vec3(2.0, 0.75, 0.01));
		windowBox.setPosition(glm::vec3(-1.5, 1.5f, -5));
		windowBox.render();
		//--------------------------------------------
		glfwSwapBuffers(window);

	}
}

int main(int argc, char ** argv) {
	init(800, 700, "Window GLFW", false);
	applicationLoop();
	destroy();
	return 1;
}