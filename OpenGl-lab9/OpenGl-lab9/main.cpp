//
//  main.cpp
//  OpenGL Advances Lighting
//
//  Created by CGIS on 28/11/16.
//  Copyright © 2016 CGIS. All rights reserved.
//

#if defined(__APPLE__)
#define GLFW_INCLUDE_GLCOREARB
#define GL_SILENCE_DEPRECATION
#else
#define GLEW_STATIC
#include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.hpp"
#include "Model3D.hpp"
#include "Camera.hpp"
#include "CelestialBody.h"
#include "Moon.h"
#include "Rocket.h"
#include "Rain.hpp"

#include <iostream>

int glWindowWidth = 800;
int glWindowHeight = 600;
int retina_width, retina_height;
GLFWwindow *glWindow = NULL;

const unsigned int SHADOW_WIDTH = 2048;
const unsigned int SHADOW_HEIGHT = 2048;

glm::mat4 model;
GLuint modelLoc;
glm::mat4 view;
GLuint viewLoc;
glm::mat4 projection;
GLuint projectionLoc;
glm::mat3 normalMatrix;
GLuint normalMatrixLoc;
glm::mat4 lightRotation;

glm::vec3 lightDir;
GLuint lightDirLoc;
glm::vec3 lightColor;
GLuint lightColorLoc;

// Add with other global variables at the top
bool isNightTime = false; // Start with daytime (point lights)

// Add with other global variables at the top
const glm::vec3 PLATFORM_POSITION = glm::vec3(0.0f, -300.0f, 0.0f);

gps::Camera myCamera(
	glm::vec3(0.0f, 2.0f, -100.5f),
	glm::vec3(0.0f, 0.0f, 0.0f),
	glm::vec3(0.0f, 1.0f, 0.0f),
	PLATFORM_POSITION.y + 1.0f // Set minimum height 1 unit above platform
);
float cameraSpeed = 0.1f;
float sprintMultiplier = 5.5f;

bool pressedKeys[1024];
float angleY = 0.0f;
GLfloat lightAngle;

gps::Model3D nanosuit;
gps::Model3D ground;
gps::Model3D lightCube;
gps::Model3D screenQuad;
gps::Model3D skybox;
gps::Model3D platform;
gps::Model3D lantern;

gps::Shader myCustomShader;
gps::Shader lightShader;
gps::Shader screenQuadShader;
gps::Shader depthMapShader;
gps::Shader sunShader;

GLuint shadowMapFBO;
GLuint depthMapTexture;

bool showDepthMap;

bool firstMouse = true;
float lastX = glWindowWidth / 2.0f;
float lastY = glWindowHeight / 2.0f;
float yaw = -90.0f;
float pitch = 0.0f;

bool isFullscreen = false;
GLFWmonitor *currentMonitor = nullptr;

gps::CelestialBody *sun = nullptr; // Global declaration
gps::CelestialBody *mercury = nullptr;
gps::CelestialBody *mars = nullptr;
gps::CelestialBody *earth = nullptr;
gps::CelestialBody *jupiter = nullptr;
gps::CelestialBody *saturn = nullptr;
gps::CelestialBody *neptune = nullptr;
gps::Moon *moon = nullptr;
gps::CelestialBody *uranus = nullptr;
gps::CelestialBody *venus = nullptr;

float lastFrame = 0.0f;				// Add at the top with other globals
float globalSpeedMultiplier = 0.0f; // Start with planets not moving

// Add at the top with other globals
int trackedPlanetIndex = -1;		  // -1 means free camera
const float TRACKING_DISTANCE = 5.0f; // Distance from camera to planet while tracking

// Create a rocket instance
gps::Rocket *rocket = nullptr;

// Add with other global variables
Rain *rain = nullptr;
gps::Shader rainShader;

// Add these as global variables at the top with other globals
glm::vec3 windDirection(0.0f);
float windStrength = 0.0f;
const float WIND_SPEED = 10.0f;

// Add to your global variables
GLuint lightColor1Loc;
GLuint lightPosLoc;
glm::vec3 lightColor1;
glm::vec3 lightPos;

// Add these with your other light-related declarations
glm::vec3 lightPos1;
glm::vec3 lightPos2;
glm::vec3 lightPos3;
glm::vec3 lightPos4;

// Add these with your other uniform location declarations
GLuint lightPosLoc1;
GLuint lightPosLoc2;
GLuint lightPosLoc3;
GLuint lightPosLoc4;

// Add this function to handle camera tracking
void updateCameraTracking()
{
	if (trackedPlanetIndex == -1)
		return; // Free camera mode

	gps::CelestialBody *targetPlanet = nullptr;
	float distanceMultiplier = 5.0f; // Base distance multiplier
	float verticalOffset = 0.0f;	 // Default vertical offset

	switch (trackedPlanetIndex)
	{
	case 1:
		targetPlanet = mercury;
		distanceMultiplier = 7.0f;
		break;
	case 2:
		targetPlanet = venus;
		distanceMultiplier = 7.0f;
		break;
	case 3:
		targetPlanet = earth;
		distanceMultiplier = 7.0f;
		break;
	case 4:
		targetPlanet = mars;
		distanceMultiplier = 7.0f;
		break;
	case 5:
		targetPlanet = jupiter;
		distanceMultiplier = 7.0f; // Larger for Jupiter
		break;
	case 6:
		targetPlanet = saturn;
		distanceMultiplier = 5.0f; // Larger for Saturn
		verticalOffset = 3.0f;
		break;
	case 7:
		targetPlanet = uranus;
		distanceMultiplier = 7.0f; // Adjusted for Uranus
		break;
	case 8:
		targetPlanet = neptune;
		distanceMultiplier = 7.0f; // Adjusted for Neptune
		break;
	case 0:
		trackedPlanetIndex = -1; // Reset to free camera
		return;
	}

	if (targetPlanet)
	{
		glm::vec3 planetPos = targetPlanet->getPosition();
		glm::vec3 sunPos = sun->getPosition();

		// Calculate direction from sun to planet
		glm::vec3 sunToPlanet = glm::normalize(planetPos - sunPos);

		// Calculate tracking distance based on planet's scale
		float trackingDistance = targetPlanet->getScale() * distanceMultiplier;

		// Position camera behind planet with vertical offset
		glm::vec3 cameraPos = planetPos + sunToPlanet * trackingDistance;
		cameraPos.y += verticalOffset; // Add vertical offset

		// Update camera position and target
		myCamera.setPosition(cameraPos);
		myCamera.setTarget(planetPos);
	}
}

// Add scroll callback function declaration at the top with other callbacks
void scrollCallback(GLFWwindow *window, double xoffset, double yoffset);

GLenum glCheckError_(const char *file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:
			error = "INVALID_ENUM";
			break;
		case GL_INVALID_VALUE:
			error = "INVALID_VALUE";
			break;
		case GL_INVALID_OPERATION:
			error = "INVALID_OPERATION";
			break;
		case GL_OUT_OF_MEMORY:
			error = "OUT_OF_MEMORY";
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			error = "INVALID_FRAMEBUFFER_OPERATION";
			break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow *window, int width, int height)
{
	fprintf(stdout, "Window resized to width: %d, height: %d\n", width, height);

	glfwGetFramebufferSize(window, &retina_width, &retina_height);
	glViewport(0, 0, retina_width, retina_height);

	projection = glm::perspective(glm::radians(45.0f),
								  (float)retina_width / (float)retina_height,
								  0.1f, 1000.0f);

	// Update projection matrix in shaders
	myCustomShader.useShaderProgram();
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	lightShader.useShaderProgram();
	glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"),
					   1, GL_FALSE, glm::value_ptr(projection));
}

// Add at the top with other global variables
enum ViewMode
{
	SOLID,
	WIREFRAME,
	POINT,
	SMOOTH,
	FLAT
};
ViewMode currentViewMode = SOLID;

void keyboardCallback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_M && action == GLFW_PRESS)
		showDepthMap = !showDepthMap;

	if (key == GLFW_KEY_F11 && action == GLFW_PRESS)
	{
		isFullscreen = !isFullscreen;

		if (isFullscreen)
		{
			// Get the primary monitor
			currentMonitor = glfwGetPrimaryMonitor();
			const GLFWvidmode *mode = glfwGetVideoMode(currentMonitor);

			// Store window position and size before going fullscreen
			glfwGetWindowPos(window, &glWindowWidth, &glWindowHeight);

			// Switch to fullscreen
			glfwSetWindowMonitor(window, currentMonitor, 0, 0,
								 mode->width, mode->height,
								 mode->refreshRate);
		}
		else
		{
			// Return to windowed mode
			const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

			// Center the window
			int centerX = (mode->width - glWindowWidth) / 2;
			int centerY = (mode->height - glWindowHeight) / 2;

			glfwSetWindowMonitor(window, nullptr, centerX, centerY,
								 glWindowWidth, glWindowHeight, 0);
		}

		// Update viewport and projection matrix for new window size
		glfwGetFramebufferSize(window, &retina_width, &retina_height);
		glViewport(0, 0, retina_width, retina_height);

		projection = glm::perspective(glm::radians(45.0f),
									  (float)retina_width / (float)retina_height,
									  0.1f, 1000.0f);

		// Update projection matrix in shaders
		myCustomShader.useShaderProgram();
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

		lightShader.useShaderProgram();
		glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"),
						   1, GL_FALSE, glm::value_ptr(projection));
	}

	if (key == GLFW_KEY_COMMA && action == GLFW_PRESS)
	{
		// Decrease global speed
		globalSpeedMultiplier = std::max(0.0f, globalSpeedMultiplier - 0.2f);
	}

	if (key == GLFW_KEY_PERIOD && action == GLFW_PRESS)
	{
		// Increase global speed
		globalSpeedMultiplier += 0.2f;
	}

	if (key == GLFW_KEY_R && action == GLFW_PRESS)
	{
		std::cout << "Launching rocket..." << std::endl; // Add debug output
		if (rocket != nullptr)
		{
			rocket->launch(glm::vec3(0.0f, 1.0f, 0.0f), 50.0f);
		}
	}

	if (key == GLFW_KEY_V && action == GLFW_PRESS)
	{
		switch (currentViewMode)
		{
		case SOLID:
			currentViewMode = WIREFRAME;
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glDisable(GL_POLYGON_SMOOTH);
			std::cout << "Wireframe mode" << std::endl;
			break;

		case WIREFRAME:
			currentViewMode = POINT;
			glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
			glDisable(GL_POLYGON_SMOOTH);
			std::cout << "Point mode" << std::endl;
			break;

		case POINT:
			currentViewMode = SMOOTH;
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glShadeModel(GL_SMOOTH);
			glEnable(GL_POLYGON_SMOOTH);
			glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
			std::cout << "Smooth shading mode" << std::endl;
			break;

		case SMOOTH:
			currentViewMode = FLAT;
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glShadeModel(GL_FLAT);
			glDisable(GL_POLYGON_SMOOTH);
			std::cout << "Flat shading mode" << std::endl;
			break;

		case FLAT:
			currentViewMode = SOLID;
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glShadeModel(GL_SMOOTH);
			glDisable(GL_POLYGON_SMOOTH);
			std::cout << "Solid mode" << std::endl;
			break;
		}
	}

	if (key == GLFW_KEY_P && action == GLFW_PRESS)
	{
		rain->toggle();
		std::cout << (rain->isEnabled() ? "Rain started" : "Rain stopped") << std::endl;
	}

	if (key == GLFW_KEY_N && action == GLFW_PRESS)
	{
		isNightTime = !isNightTime; // Toggle between day and night

		myCustomShader.useShaderProgram();
		// Update the shader about the current state
		glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "isNightTime"), isNightTime);
	}

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			pressedKeys[key] = true;
		else if (action == GLFW_RELEASE)
			pressedKeys[key] = false;
	}

	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_0:
			trackedPlanetIndex = -1; // Free camera
			break;
		case GLFW_KEY_1:
			trackedPlanetIndex = 1; // Mercury
			break;
		case GLFW_KEY_2:
			trackedPlanetIndex = 2; // Venus
			break;
		case GLFW_KEY_3:
			trackedPlanetIndex = 3; // Earth
			break;
		case GLFW_KEY_4:
			trackedPlanetIndex = 4; // Mars
			break;
		case GLFW_KEY_5:
			trackedPlanetIndex = 5; // Jupiter
			break;
		case GLFW_KEY_6:
			trackedPlanetIndex = 6; // Saturn
			break;
		case GLFW_KEY_7:
			trackedPlanetIndex = 7; // Uranus
			break;
		case GLFW_KEY_8:
			trackedPlanetIndex = 8; // Neptune
			break;
		}
	}
}

void mouseCallback(GLFWwindow *window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
		return;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	const float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	// Constrain pitch
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	myCamera.rotate(pitch, yaw);
}

void scrollCallback(GLFWwindow *window, double xoffset, double yoffset)
{
	myCamera.zoom(yoffset);

	// Update projection matrix with camera's FOV
	projection = glm::perspective(glm::radians(myCamera.getFov()),
								  (float)retina_width / (float)retina_height,
								  0.1f, 1000.0f);

	// Update projection matrix in shaders
	myCustomShader.useShaderProgram();
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	lightShader.useShaderProgram();
	glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"),
					   1, GL_FALSE, glm::value_ptr(projection));
}

void processMovement()
{
	float currentSpeed = cameraSpeed;
	if (pressedKeys[GLFW_KEY_LEFT_SHIFT])
	{
		currentSpeed *= sprintMultiplier; // Sprint when holding shift
	}

	if (pressedKeys[GLFW_KEY_Q])
	{
		angleY -= 1.0f;
	}

	if (pressedKeys[GLFW_KEY_E])
	{
		angleY += 1.0f;
	}

	if (pressedKeys[GLFW_KEY_J])
	{
		lightAngle -= 1.0f;
	}

	if (pressedKeys[GLFW_KEY_L])
	{
		lightAngle += 1.0f;
	}

	if (pressedKeys[GLFW_KEY_W])
	{
		myCamera.move(gps::MOVE_FORWARD, currentSpeed);
	}

	if (pressedKeys[GLFW_KEY_S])
	{
		myCamera.move(gps::MOVE_BACKWARD, currentSpeed);
	}

	if (pressedKeys[GLFW_KEY_A])
	{
		myCamera.move(gps::MOVE_LEFT, currentSpeed);
	}

	if (pressedKeys[GLFW_KEY_D])
	{
		myCamera.move(gps::MOVE_RIGHT, currentSpeed);
	}

	// Wind controls using arrow keys
	if (pressedKeys[GLFW_KEY_UP])
	{ // Wind from North
		windDirection = glm::vec3(0.0f, 0.0f, -1.0f);
		windStrength = WIND_SPEED;
	}
	else if (pressedKeys[GLFW_KEY_DOWN])
	{ // Wind from South
		windDirection = glm::vec3(0.0f, 0.0f, 1.0f);
		windStrength = WIND_SPEED;
	}
	else if (pressedKeys[GLFW_KEY_LEFT])
	{ // Wind from West
		windDirection = glm::vec3(-1.0f, 0.0f, 0.0f);
		windStrength = WIND_SPEED;
	}
	else if (pressedKeys[GLFW_KEY_RIGHT])
	{ // Wind from East
		windDirection = glm::vec3(1.0f, 0.0f, 0.0f);
		windStrength = WIND_SPEED;
	}
	else
	{
		windStrength = 0.0f; // No wind when no key is pressed
	}

	// Update view matrix
	view = myCamera.getViewMatrix();
	myCustomShader.useShaderProgram();
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
}

bool initOpenGLWindow()
{
	if (!glfwInit())
	{
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// window scaling for HiDPI displays
	glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);

	// for sRBG framebuffer
	glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);

	// for antialising
	glfwWindowHint(GLFW_SAMPLES, 4);

	glWindow = glfwCreateWindow(glWindowWidth, glWindowHeight, "OpenGL Shader Example", NULL, NULL);
	if (!glWindow)
	{
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return false;
	}

	glfwSetWindowSizeCallback(glWindow, windowResizeCallback);
	glfwSetKeyCallback(glWindow, keyboardCallback);
	glfwSetCursorPosCallback(glWindow, mouseCallback);
	glfwSetScrollCallback(glWindow, scrollCallback);
	// glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwMakeContextCurrent(glWindow);

	glfwSwapInterval(1);

#if not defined(__APPLE__)
	// start GLEW extension handler
	glewExperimental = GL_TRUE;
	glewInit();
#endif

	// get version info
	const GLubyte *renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte *version = glGetString(GL_VERSION);	// version as a string
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	// for RETINA display
	glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

	// Set up mouse input
	glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	return true;
}

void initOpenGLState()
{
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glViewport(0, 0, retina_width, retina_height);

	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS);	 // depth-testing interprets a smaller value as "closer"
	glEnable(GL_CULL_FACE);	 // cull face
	glCullFace(GL_BACK);	 // cull back face
	glFrontFace(GL_CCW);	 // GL_CCW for counter clock-wise

	glEnable(GL_FRAMEBUFFER_SRGB);

	// Set initial view mode and smooth shading
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glShadeModel(GL_SMOOTH); // Enable smooth shading by default
	glEnable(GL_POLYGON_SMOOTH);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
}

void initObjects()
{
	float scale = 1.0f;

	nanosuit.LoadModel("objects/nanosuit/nanosuit.obj");
	ground.LoadModel("objects/ground/ground.obj");
	lightCube.LoadModel("objects/cube/cube.obj");
	screenQuad.LoadModel("objects/quad/quad.obj");
	skybox.LoadModel("objects/universe/map.obj");
	platform.LoadModel("objects/platform/platform.obj");
	lantern.LoadModel("objects/lantern/12299_Lantern_v1_l2.obj");

	// Initialize rocket with a smaller scale
	rocket = new gps::Rocket("objects/rocket/apollo.obj",
							 PLATFORM_POSITION,
							 5.0f); // Reduced scale to 0.1

	// Sun at center, radius is 109 units
	sun = new gps::CelestialBody("objects/sun/sun.obj",
								 glm::vec3(0.0f, 0.0f, 0.0f),
								 109.0f / scale, 0.0f, 0.0f);

	// All orbital distances must be > 109 units
	mercury = new gps::CelestialBody("objects/mercury/mercury.obj",
									 glm::vec3(140.0f / scale, 0.0f, 0.0f), // Moved out to 140
									 0.383f / scale, 140.0f / scale, 1.6f);

	venus = new gps::CelestialBody("objects/venus/venus.obj",
								   glm::vec3(170.0f / scale, 0.0f, 0.0f), // Moved out to 170
								   0.949f / scale, 170.0f / scale, 1.2f);

	earth = new gps::CelestialBody("objects/earth/earth.obj",
								   glm::vec3(200.0f / scale, 0.0f, 0.0f), // Moved out to 200
								   1.0f / scale, 200.0f / scale, 1.0f);

	mars = new gps::CelestialBody("objects/mars/mars.obj",
								  glm::vec3(230.0f / scale, 0.0f, 0.0f), // Moved out to 230
								  0.532f / scale, 230.0f / scale, 0.8f);

	jupiter = new gps::CelestialBody("objects/jupiter/jupiter.obj",
									 glm::vec3(280.0f / scale, 0.0f, 0.0f), // Moved out to 280
									 11.209f / scale, 280.0f / scale, 0.4f);

	saturn = new gps::CelestialBody("objects/saturn/saturn.obj",
									glm::vec3(330.0f / scale, 0.0f, 0.0f), // Moved out to 330
									9.449f / scale, 330.0f / scale, 0.3f);

	uranus = new gps::CelestialBody("objects/uranus/uranus.obj",
									glm::vec3(380.0f / scale, 0.0f, 0.0f), // Moved out to 380
									4.007f / scale, 380.0f / scale, 0.2f);

	neptune = new gps::CelestialBody("objects/neptune/neptune.obj",
									 glm::vec3(430.0f / scale, 0.0f, 0.0f), // Moved out to 430
									 3.883f / scale, 430.0f / scale, 0.1f);

	moon = new gps::Moon("objects/moon/moon.obj",
						 glm::vec3(203.0f / scale, 0.0f, 0.0f), // Slightly beyond Earth
						 0.273f / scale, 3.0f / scale, 2.0f);
	moon->setParentPlanet(earth);

	rain = new Rain(PLATFORM_POSITION);
}

void initShaders()
{
	myCustomShader.loadShader("shaders/shaderStart.vert", "shaders/shaderStart.frag");
	myCustomShader.useShaderProgram();

	lightShader.loadShader("shaders/lightCube.vert", "shaders/lightCube.frag");
	lightShader.useShaderProgram();

	screenQuadShader.loadShader("shaders/screenQuad.vert", "shaders/screenQuad.frag");
	screenQuadShader.useShaderProgram();

	depthMapShader.loadShader("shaders/depthMap.vert", "shaders/depthMap.frag");
	depthMapShader.useShaderProgram();

	// Add sun shader initialization
	sunShader.loadShader("shaders/sunVertex.vert", "shaders/sunFragment.frag");
	sunShader.useShaderProgram();

	rainShader.loadShader("shaders/rain.vert", "shaders/rain.frag");
}

void initUniforms()
{
	myCustomShader.useShaderProgram();

	model = glm::mat4(1.0f);
	modelLoc = glGetUniformLocation(myCustomShader.shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	view = myCamera.getViewMatrix();
	viewLoc = glGetUniformLocation(myCustomShader.shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
	normalMatrixLoc = glGetUniformLocation(myCustomShader.shaderProgram, "normalMatrix");
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	projectionLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	// set the light direction (direction towards the light)
	lightDir = glm::vec3(0.0f, 1.0f, 1.0f);
	lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
	lightDirLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDir");
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));

	// set light color
	lightColor = glm::vec3(1.0f, 0.9f, 0.5f); // warm yellow light
	lightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor");
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

	lightShader.useShaderProgram();
	glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	// Add point light setup
	lightColor1 = glm::vec3(1.0f, 0.0f, 0.0f); // red point light
	lightColor1Loc = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor1");
	glUniform3fv(lightColor1Loc, 1, glm::value_ptr(lightColor1));

	// Position the point light (e.g., near the main tree)
	lightPos = glm::vec3(-3.0f, 0.0f, -13.0f);
	lightPosLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightPos");
	glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightPos));

	// Set up multiple point lights around the platform
	glm::vec3 yellowLight = glm::vec3(1.0f, 1.0f, 0.0f); // Yellow color

	// Light positions relative to PLATFORM_POSITION
	lightPos = PLATFORM_POSITION + glm::vec3(-20.0f, 10.0f, -20.0f); // Front left
	lightPos1 = PLATFORM_POSITION + glm::vec3(20.0f, 10.0f, -20.0f); // Front right
	lightPos2 = PLATFORM_POSITION + glm::vec3(-20.0f, 10.0f, 20.0f); // Back left
	lightPos3 = PLATFORM_POSITION + glm::vec3(20.0f, 10.0f, 20.0f);	 // Back right
	lightPos4 = PLATFORM_POSITION + glm::vec3(0.0f, 15.0f, 0.0f);	 // Center, higher up

	// Set light positions in shader
	myCustomShader.useShaderProgram();
	glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightPos));
	glUniform3fv(lightPosLoc1, 1, glm::value_ptr(lightPos1));
	glUniform3fv(lightPosLoc2, 1, glm::value_ptr(lightPos2));
	glUniform3fv(lightPosLoc3, 1, glm::value_ptr(lightPos3));
	glUniform3fv(lightPosLoc4, 1, glm::value_ptr(lightPos4));

	// Set light colors (all yellow in this case)
	glUniform3fv(lightColor1Loc, 1, glm::value_ptr(yellowLight));
	glUniform3fv(glGetUniformLocation(myCustomShader.shaderProgram, "lightColor2"), 1, glm::value_ptr(yellowLight));
	glUniform3fv(glGetUniformLocation(myCustomShader.shaderProgram, "lightColor3"), 1, glm::value_ptr(yellowLight));
	glUniform3fv(glGetUniformLocation(myCustomShader.shaderProgram, "lightColor4"), 1, glm::value_ptr(yellowLight));
	glUniform3fv(glGetUniformLocation(myCustomShader.shaderProgram, "lightColor5"), 1, glm::value_ptr(yellowLight));

	// Get uniform locations for all light positions
	lightPosLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightPos");
	lightPosLoc1 = glGetUniformLocation(myCustomShader.shaderProgram, "lightPos1");
	lightPosLoc2 = glGetUniformLocation(myCustomShader.shaderProgram, "lightPos2");
	lightPosLoc3 = glGetUniformLocation(myCustomShader.shaderProgram, "lightPos3");
	lightPosLoc4 = glGetUniformLocation(myCustomShader.shaderProgram, "lightPos4");

	// Set light positions
	lightPos = PLATFORM_POSITION + glm::vec3(-20.0f, 10.0f, -20.0f); // Front left
	lightPos1 = PLATFORM_POSITION + glm::vec3(20.0f, 10.0f, -20.0f); // Front right
	lightPos2 = PLATFORM_POSITION + glm::vec3(-20.0f, 10.0f, 20.0f); // Back left
	lightPos3 = PLATFORM_POSITION + glm::vec3(20.0f, 10.0f, 20.0f);	 // Back right
	lightPos4 = PLATFORM_POSITION + glm::vec3(0.0f, 15.0f, 0.0f);	 // Center, higher up

	// Set the uniforms
	glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightPos));
	glUniform3fv(lightPosLoc1, 1, glm::value_ptr(lightPos1));
	glUniform3fv(lightPosLoc2, 1, glm::value_ptr(lightPos2));
	glUniform3fv(lightPosLoc3, 1, glm::value_ptr(lightPos3));
	glUniform3fv(lightPosLoc4, 1, glm::value_ptr(lightPos4));

	// Initialize the day/night state
	myCustomShader.useShaderProgram();
	glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "isNightTime"), isNightTime);
}

void initFBO()
{
	// TODO - Create the FBO, the depth texture and attach the depth texture to the FBO
	glGenFramebuffers(1, &shadowMapFBO);
	glGenTextures(1, &depthMapTexture);
	glBindTexture(GL_TEXTURE_2D, depthMapTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);

	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

glm::mat4 computeLightSpaceTrMatrix()
{
	// TODO - Return the light-space transformation matrix
	lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));

	glm::mat4 lightView = glm::lookAt(glm::vec3(lightRotation * glm::vec4(lightDir, 1.0f)), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	const GLfloat near_plane = 0.5f, far_plane = 6.0f;
	glm::mat4 lightProjection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, near_plane, far_plane);
	glm::mat4 lightSpaceTrMatrix = lightProjection * lightView;

	return lightSpaceTrMatrix;
}

void drawObjects(gps::Shader shader, bool depthPass)
{
	// Draw skybox first
	glDepthFunc(GL_LEQUAL);
	model = glm::scale(glm::mat4(1.0f), glm::vec3(500.0f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	skybox.Draw(shader);
	glDepthFunc(GL_LESS);

	// Handle sun drawing
	if (!depthPass && shader.shaderProgram == myCustomShader.shaderProgram)
	{
		// Switch to sun shader
		sunShader.useShaderProgram();

		// Set uniforms for sun shader
		model = glm::mat4(1.0f);
		glUniformMatrix4fv(glGetUniformLocation(sunShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(glGetUniformLocation(sunShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(sunShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniform1f(glGetUniformLocation(sunShader.shaderProgram, "time"), glfwGetTime());

		// Draw sun
		sun->draw(sunShader);

		// Switch back to regular shader
		shader.useShaderProgram();
	}
	else
	{
		sun->draw(shader);
	}

	// Draw remaining objects with regular shader
	mercury->draw(shader);
	venus->draw(shader);
	earth->draw(shader);
	mars->draw(shader);
	jupiter->draw(shader);
	saturn->draw(shader);
	uranus->draw(shader);
	neptune->draw(shader);
	moon->draw(shader);

	// Draw rocket
	if (rocket != nullptr)
	{
		rocket->draw(shader);
	}

	// Draw platform with regular shader
	model = glm::mat4(1.0f);
	model = glm::translate(model, PLATFORM_POSITION);
	model = glm::scale(model, glm::vec3(100.0f, 0.1f, 100.0f));
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	platform.Draw(shader);

	// Draw lantern
	model = glm::mat4(1.0f);
	model = glm::translate(model, PLATFORM_POSITION + glm::vec3(-20.0f, 10.0f, 0.0f)); // 5 units above platform
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));	   // Rotate 90 degrees around X axis
	model = glm::scale(model, glm::vec3(0.1f));										   // Adjust scale as needed
	glUniformMatrix4fv(glGetUniformLocation(shader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	lantern.Draw(shader);
}

void renderScene()
{
	float currentFrame = glfwGetTime();
	float deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	// Update planet positions
	mercury->update(deltaTime, globalSpeedMultiplier);
	venus->update(deltaTime, globalSpeedMultiplier);
	earth->update(deltaTime, globalSpeedMultiplier);
	mars->update(deltaTime, globalSpeedMultiplier);
	jupiter->update(deltaTime, globalSpeedMultiplier);
	saturn->update(deltaTime, globalSpeedMultiplier);
	uranus->update(deltaTime, globalSpeedMultiplier);
	neptune->update(deltaTime, globalSpeedMultiplier);
	moon->update(deltaTime, globalSpeedMultiplier);

	// Update rocket (remove the duplicate update call)
	rocket->update(deltaTime, 1.0f); // Use 1.0f instead of globalSpeedMultiplier for consistent physics

	// Update camera tracking before rendering
	updateCameraTracking();

	// depth maps creation pass
	// TODO - Send the light-space transformation matrix to the depth map creation shader and
	//		 render the scene in the depth map
	depthMapShader.useShaderProgram();
	glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix"), 1, GL_FALSE, glm::value_ptr(computeLightSpaceTrMatrix()));
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	drawObjects(depthMapShader, true);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// render depth map on screen - toggled with the M key

	if (showDepthMap)
	{
		glViewport(0, 0, retina_width, retina_height);

		glClear(GL_COLOR_BUFFER_BIT);

		screenQuadShader.useShaderProgram();

		// bind the depth map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMapTexture);
		glUniform1i(glGetUniformLocation(screenQuadShader.shaderProgram, "depthMap"), 0);

		glDisable(GL_DEPTH_TEST);
		screenQuad.Draw(screenQuadShader);
		glEnable(GL_DEPTH_TEST);
	}
	else
	{

		// final scene rendering pass (with shadows)

		glViewport(0, 0, retina_width, retina_height);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		myCustomShader.useShaderProgram();

		view = myCamera.getViewMatrix();
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(lightAngle), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));

		// bind the shadow map
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, depthMapTexture);
		glUniform1i(glGetUniformLocation(myCustomShader.shaderProgram, "shadowMap"), 3);

		glUniformMatrix4fv(glGetUniformLocation(myCustomShader.shaderProgram, "lightSpaceTrMatrix"),
						   1,
						   GL_FALSE,
						   glm::value_ptr(computeLightSpaceTrMatrix()));

		drawObjects(myCustomShader, false);

		// draw a white cube around the light

		lightShader.useShaderProgram();

		glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

		model = lightRotation;
		model = glm::translate(model, 1.0f * lightDir);
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		glUniformMatrix4fv(glGetUniformLocation(lightShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

		lightCube.Draw(lightShader);
	}

	// Set point size for point mode
	if (currentViewMode == POINT)
	{
		glPointSize(3.0f);
	}

	if (rain->isEnabled())
	{
		rainShader.useShaderProgram();

		// Pass wind uniforms to shader
		glUniform3fv(glGetUniformLocation(rainShader.shaderProgram, "windDirection"), 1, glm::value_ptr(windDirection));
		glUniform1f(glGetUniformLocation(rainShader.shaderProgram, "windStrength"), windStrength);

		glUniformMatrix4fv(glGetUniformLocation(rainShader.shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(rainShader.shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDepthMask(GL_FALSE);

		rain->update(deltaTime, windDirection, windStrength);
		rain->draw();

		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);
	}

	myCustomShader.useShaderProgram();

	// Update light position uniform (if the light moves)
	glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightPos));
}

void cleanup()
{
	glDeleteTextures(1, &depthMapTexture);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &shadowMapFBO);
	glfwDestroyWindow(glWindow);
	// close GL context and any other GLFW resources
	glfwTerminate();

	delete sun;
	delete mercury;
	delete mars;
	delete earth;
	delete jupiter;
	delete saturn;
	delete neptune;
	delete moon;
	delete uranus;
	delete venus;
	delete rocket;
	delete rain;
}

int main(int argc, const char *argv[])
{

	if (!initOpenGLWindow())
	{
		glfwTerminate();
		return 1;
	}

	initOpenGLState();
	initObjects();
	initShaders();
	initUniforms();
	initFBO();

	glCheckError();

	while (!glfwWindowShouldClose(glWindow))
	{
		processMovement();
		renderScene();

		glfwPollEvents();
		glfwSwapBuffers(glWindow);
	}

	cleanup();

	return 0;
}
