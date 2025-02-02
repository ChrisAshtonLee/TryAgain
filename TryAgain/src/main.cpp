#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <sstream>
#include <streambuf>
#include <string>
#include "rendering/shader.h"
#include "rendering/uniformmemory.hpp"
#include "programs/halfspace.hpp"
#include "programs/rectangle.hpp"
#include "programs/sphere.hpp"
#include "programs/line.hpp"
#include "io/camera.h"
#include "io/keyboard.h"
#include "io/mouse.h"


std::string Shader::defaultDirectory = "assets/shaders";
void initGLFW(unsigned int versionMajor, unsigned int versionMinor);
void createWindow(GLFWwindow*& window, const char* title, unsigned int width, unsigned int height, GLFWframebuffersizefun framebufferSizeCallback);
//callbacks
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void processInput(double dt);
void updateCameraMatrices();
void keyChanged(GLFWwindow* window, int key, int scancode, int action, int mods);
void cursorChanged(GLFWwindow* window, double _x, double _y);
void mouseButtonChanged(GLFWwindow* window, int button, int action, int mods);
void scrollChanged(GLFWwindow* window, double dx, double dy);
void characterCallback(GLFWwindow* window, unsigned int keyCode);
//global vars
int scr_width = 800, scr_height = 800;
GLFWwindow* window = nullptr;
//Camera
Camera cam(glm::vec3(-2.0f, 0.0f, 0.0f));
glm::mat4 view;
glm::mat4 projection;

//Programs
std::vector<Program*> programs;
std::vector<Halfspace*> spaces;
//Rectangle rect;
Sphere sphere(1);

Line line(glm::vec3(1.0, 0.0, 0.5), glm::vec3(9.0, 0.0, 0.5));
glm::vec3 color1 = { 1.0,0.0,0.0 };
glm::vec3 color2 = { 0.0,1.0,0.0 };
Halfspace halfspace(0.1f, 1.0f,0.0f,color1);
Halfspace halfspace2(0.5f, 0.5f,0.5f,color2);


typedef struct {
	glm::vec3 dir;
	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;
} DirLight;

int main() {
	//init
	initGLFW(3, 3);
	createWindow(window, "MathPlot", scr_width, scr_height, framebufferSizeCallback);
	if (!window) {
		std::cout << "Couldn't create window" << std::endl;
		glfwTerminate();
		return -1;
	}

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Couldn't load GLAD" << std::endl;
		glfwTerminate();
		return -1;
	}
	//window params

	glfwSwapInterval(1);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	
	//rendering params
	glEnable(GL_DEPTH_TEST);
	glViewport(0, 0,scr_width, scr_height);

	//io callbacks
	glfwSetKeyCallback(window, Keyboard::keyCallback);
	glfwSetCursorPosCallback(window, Mouse::cursorPosCallback);
	glfwSetMouseButtonCallback(window, Mouse::mouseButtonCallback);
	glfwSetScrollCallback(window, Mouse::mouseWheelCallback);
	Keyboard::keyCallbacks.push_back(keyChanged);
	Mouse::cursorPosCallbacks.push_back(cursorChanged);
	Mouse::mouseButtonCallbacks.push_back(mouseButtonChanged);
	Mouse::mouseWheelCallbacks.push_back(scrollChanged);
	glfwSetCharCallback(window, characterCallback);

	//generate instances
	sphere.addInstance(glm::vec3(1.0f), glm::vec3(0.2f), Material::bronze);
	programs.push_back(&sphere);
	//lighting
	DirLight dirLight = {
		glm::vec3(-0.2f,-0.9f,-0.2f),
		glm::vec4(0.5f,0.5f,0.5f,1.0f),
		glm::vec4(0.75f,0.75f,0.75f,1.0f),
		glm::vec4(1.0f,1.0f,1.0f,1.0f)
	};
	UBO::UBO dirLightUBO({
		UBO::newStruct({
			UBO::Type::VEC3,
			UBO::Type::VEC4,
			UBO::Type::VEC4,
			UBO::Type::VEC4
		})
	});

	for (Program* program : programs)
	{
		dirLightUBO.attachToShader(program->shader, "DirLightUniform");
	}
	
	dirLightUBO.generate();
	dirLightUBO.bind();
	dirLightUBO.initNullData(GL_STATIC_DRAW);
	dirLightUBO.bindRange();

	dirLightUBO.startWrite();
	dirLightUBO.writeElement<glm::vec3>(&dirLight.dir);
	dirLightUBO.writeElement<glm::vec4>(&dirLight.ambient);
	dirLightUBO.writeElement<glm::vec4>(&dirLight.diffuse);
	dirLightUBO.writeElement<glm::vec4>(&dirLight.specular);

	//timing vars
	double dt = 0.0;
	double lastFrame = 0.0;
	//setup programs
	//rect.load();
	sphere.load();
	line.load();
	halfspace.load();
	halfspace2.load();
	spaces.push_back(&halfspace);
	spaces.push_back(&halfspace2);
	updateCameraMatrices();

	while (!glfwWindowShouldClose(window)) {
		//update time
		dt = glfwGetTime() - lastFrame;
		lastFrame += dt;

		//input
		glfwWaitEventsTimeout(0.001);
		processInput(dt);
		//render
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//rect.render(dt);
		
		sphere.render();
		//line.render();
		//halfspace.render();
		for (Halfspace* space : spaces) {
			space->render();
		}
		glfwSwapBuffers(window);

	}
	//cleanup programs
	//rect.cleanup();
	

	sphere.cleanup();
	line.cleanup();
	for (Halfspace* space : spaces) {
		space->cleanup();
	}

	//cleanup glfw
	glfwTerminate();

	return 0;
}


void initGLFW(unsigned int versionMajor, unsigned int versionMinor) {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, versionMajor);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, versionMinor);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

}
void createWindow(GLFWwindow*& window, const char* title, unsigned int width, unsigned int height, GLFWframebuffersizefun framebufferSizeCallback) {
	window = glfwCreateWindow(width, height, title, NULL, NULL);
	if (!window) {
		return;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

}
void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	scr_width = width;
	scr_height = height;
	updateCameraMatrices();
}

void processInput(double dt) {
	if (Keyboard::key(GLFW_KEY_W)) {
		cam.updateCameraPos(CameraDirection::FORWARD, dt);
		updateCameraMatrices();
	}
	if (Keyboard::key(GLFW_KEY_S)) {
		cam.updateCameraPos(CameraDirection::BACKWARD, dt);
		updateCameraMatrices();
	}
	if (Keyboard::key(GLFW_KEY_A)) {
		cam.updateCameraPos(CameraDirection::LEFT, dt);
		updateCameraMatrices();
	}
	if (Keyboard::key(GLFW_KEY_D)) {
		cam.updateCameraPos(CameraDirection::RIGHT, dt);
		updateCameraMatrices();
	}
	if (Keyboard::key(GLFW_KEY_SPACE)) {
		cam.updateCameraPos(CameraDirection::UP, dt);
		updateCameraMatrices();
	}
	if (Keyboard::key(GLFW_KEY_LEFT_SHIFT)) {
		cam.updateCameraPos(CameraDirection::DOWN, dt);
		updateCameraMatrices();
	}

}
void updateCameraMatrices() {
	view = cam.getViewMatrix();
	projection = glm::perspective(glm::radians(cam.getZoom()), (float)scr_width / (float)scr_height, 0.1f, 100.0f);

	//rect.updateCameraMatrices(projection * view, cam.cameraPos);
	sphere.updateCameraMatrices(projection * view, cam.cameraPos);
	line.updateCameraMatrices(projection * view, cam.cameraPos);
	for (Halfspace* space : spaces) {
		space->updateCameraMatrices(projection * view, cam.cameraPos);
	}
	
	
}
void keyChanged(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (Keyboard::key(GLFW_KEY_ESCAPE)) {
		glfwSetWindowShouldClose(window, true);
	}
}
void cursorChanged(GLFWwindow* window, double _x, double _y) {
	double dx = Mouse::getDX();
	double dy = Mouse::getDY();
	if ((dx != 0 || dy != 0) && Mouse::button(GLFW_MOUSE_BUTTON_RIGHT)) {
		cam.updateCameraDirection(dx, dy);
	}
	updateCameraMatrices();
}
void mouseButtonChanged(GLFWwindow* window, int button, int action, int mods) {

}
void scrollChanged(GLFWwindow* window, double dx, double dy) {
	double scrollDy = Mouse::getScrollDY();
	if (scrollDy != 0) {
		cam.updateCameraZoom(scrollDy);
	}
	updateCameraMatrices();
}
void characterCallback(GLFWwindow* window, unsigned int keyCode) {
	if (keyCode < 59 && keyCode>47) {
		std::cout << keyCode-48 << std::endl;
	}

}