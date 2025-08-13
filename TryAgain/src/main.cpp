#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <assets/image/stb_image_write.h>
#include <fstream>
#include <sstream>
#include <streambuf>
#include <string>
#include "rendering/shader.h"
#include "rendering/uniformmemory.hpp"
#include "programs/halfspace.hpp"
#include "programs/rectangle.hpp"
#include "programs/sphere.h"
#include "programs/points.hpp"
#include "programs/line.hpp"
#include "programs/polygon.hpp"
#include <assets/image/plotter.h>
#include "io/camera.h"
#include "io/keyboard.h"
#include "io/mouse.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include "ConvexHull.h"
#include "TukeyRegion.h"
#include <random>
#include <src/UI/UI.h>
#include <memory>
#include <common/data.h>
#include <iostream>
#include <scripts/ResilientConsensus.hpp>

#include<glm/gtc/type_ptr.hpp>
#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h> // Will drag system OpenGL headers
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif


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
bool imgui_wants_mouse = false;
glm::vec2 worldToScreen(const glm::vec3& worldPos, const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection, int screenWidth, int screenHeight);
glm::vec3 screenToWorld(const glm::vec2& screenPos, float winZ, const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection, int screenWidth, int screenHeight);
float mouse_x;
float mouse_y;
glm::vec2 startPos;
glm::vec2 endPos;
glm::vec2 currentPos;
bool dragging = false;
bool preview;
bool sim_init = false;
bool start_capture_done = false;
bool start_capture_pending = false;
bool end_capture_pending = false;
CameraData camData;
GLFWwindow* window = nullptr;

//GLFWwindow* imgui_window = nullptr;
//Camera
//Camera cam(glm::vec3(0.0f, 0.0f, -1.0f));
glm::mat4 view;
glm::mat4 projection;

//Programs
std::vector<std::shared_ptr<Program>> programs;
std::vector<Halfspace*> spaces;
//Rectangle rect;
//Sphere sphere(1);


glm::vec3 color1 = { 1.0,0.0,0.0 };
glm::vec3 color2 = { 0.0,1.0,0.0 };
//Halfspace halfspace;
//Points points;
//Polygon polygon;
std::shared_ptr<Camera> camPtr;
std::vector<vector<float>> pointData;
std::shared_ptr<vector<vector<float>>> pointDataPtr;
std::shared_ptr<Points> pointsPtr;
std::shared_ptr<Halfspace> halfspacePtr;
std::shared_ptr<Polygon> polygonPtr;
std::shared_ptr<Line> linePtr;
std::shared_ptr<Sphere> spherePtr;
std::shared_ptr<ResilientConsensus> rcPtr;
std::shared_ptr<Plotter> plotterPtr;
std::vector<std::pair<float, float>> random_points;

UI_DESC desc;
UI* uiPtr = nullptr;

typedef struct {
	glm::vec3 dir;
	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;
} DirLight;

int main(int, char**) {
	camPtr = std::make_shared<Camera>(glm::vec3(-2.0f, 0.0f, 0.0f));
	//camPtr = std::make_shared<Camera>(glm::vec3(0.0f, -2.0f, 0.0f));


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
	
	
	//GLFWwindow* imgui_window = glfwCreateWindow(1280, 720, "Dear ImGui GLFW+OpenGL3 example", nullptr, nullptr);
	
	if (window == nullptr)
		return 9;
	
	//init programs
	pointsPtr = std::make_shared<Points>();
	halfspacePtr = std::make_shared<Halfspace>();
	polygonPtr = std::make_shared<Polygon>();
	linePtr = std::make_shared<Line>();
	
	spherePtr = std::make_shared<Sphere>(10);
	plotterPtr = std::make_shared<Plotter>(pointsPtr,linePtr,scr_width, scr_height);
	desc.points = pointsPtr;
	desc.line = linePtr;
	desc.polygon = polygonPtr;
	desc.halfspace = halfspacePtr;
	desc.sphere = spherePtr;
	desc.rc = rcPtr;
	programs.push_back(pointsPtr);
	programs.push_back(polygonPtr);
	programs.push_back(linePtr);
	programs.push_back(spherePtr);

	glfwSwapInterval(1); // Enable vsync

	const char* glsl_version = "#version 130";
//// Setup Dear ImGui context
	uiPtr = new UI(window, glsl_version,desc);

	// Setup Platform/Renderer backends

	bool show_another_window = true;
	char input1[256] = "input1";
	char input2[256] = "input2";
	char input3[256] = "input3";
	int k_input =0;
	int n_input = 0;
	

	//Plotting
	
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	

	glfwSwapInterval(1);
	glfwSetInputMode(window, GLFW_CURSOR,GLFW_CURSOR_NORMAL);
	
	//rendering params
	glEnable(GL_DEPTH_TEST);
	glViewport(0, 0,scr_width, scr_height);
	glEnable(GL_PROGRAM_POINT_SIZE);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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
	dirLightUBO.attachToShader(spherePtr->shader, "DirLightUniform");
	//for (Program* program : programs)
	//{
		//dirLightUBO.attachToShader(program->shader, "DirLightUniform");
	//}
	
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
	int gui_mode = 0;

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dis(-1.5, 1.5);
	updateCameraMatrices();



	while (!glfwWindowShouldClose(window)) {
		
		dt = glfwGetTime() - lastFrame;
		lastFrame += dt;
		
		//input
		glfwWaitEventsTimeout(0.001);
		if (ImGui::GetIO().WantCaptureMouse) {
			imgui_wants_mouse = true;
			
		}
		else {
			imgui_wants_mouse = false;
			
		}
		processInput(dt);
		auto capture_logic = [&](const std::string& filename) {
			if (uiPtr->capture_area_set) {
				int x = std::min(uiPtr->capture_start_pos.x, uiPtr->capture_end_pos.x);
				int y = std::min(uiPtr->capture_start_pos.y, uiPtr->capture_end_pos.y);
				int width = std::abs(uiPtr->capture_start_pos.x - uiPtr->capture_end_pos.x);
				int height = std::abs(uiPtr->capture_start_pos.y - uiPtr->capture_end_pos.y);
				plotterPtr->saveFrame(filename, x, y, width, height);
			}
			else {
				plotterPtr->saveFrame(filename);
			}
			};

		if (start_capture_pending) {
			capture_logic("C:/Users/85chr/source/repos/TryAgain/plots/simulation_start.png");

			if (rcPtr) {
				rcPtr->start_sim(100);
			}
			start_capture_pending = false; // Mark as complete
		}
		if (uiPtr->sim_running && !sim_init) {
			const auto& normal_agents = uiPtr->getNormalAgents();
			const auto& adversaries = uiPtr->getAdversaries();
			rcPtr = std::make_shared<ResilientConsensus>(normal_agents, adversaries, pointsPtr);
			sim_init = true;
			//rcPtr->start_sim(100);
			start_capture_pending = true;
			std::cout << "sim started." << std::endl;
			
		}
		/*if (rcPtr && !rcPtr->is_running && !end_capture_pending)
		{
			end_capture_pending = true;
		}*/
		if (rcPtr && rcPtr->is_running) {
			rcPtr->sim_step();
			if (!rcPtr->is_running) {
				end_capture_pending = true;
			}
		}
		//else if (sim_init) {
		//	// If the simulation has finished, reset the flags
		//	uiPtr->sim_running = false;
		//	sim_init = false;
		//	 // Clear the pointer
		//}
		//update time
		uiPtr->NewFrame();
		//window params
		uiPtr->DrawWindow();
		uiPtr->DrawInspectorWindow(2);
		uiPtr->DrawInspectorWindow(1);
		//render
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		if (end_capture_pending) {
			// Use the plotter to render trajectories
			plotterPtr->renderTrajectories(rcPtr->X_history, projection, view);
		}
		for (auto& p : programs)
		{
			if (p->noInstances > 0)
			{
				p->render();
			}
		 }
		//pointsPtr->render();
		//
		//polygonPtr->render();
		//linePtr->render();
		////halfspacePtr->render();
		//spherePtr->render();
		uiPtr->Render();
		if (uiPtr->previewMode && uiPtr->previewInstance)
		{
			//pointsPtr->renderPreview();
			//uiPtr->Render();
			spherePtr->renderPreview(uiPtr->previewPos, 1.0f, projection * view);
		}
		
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
	
		if (end_capture_pending) {
			capture_logic("C:/Users/85chr/source/repos/TryAgain/plots/simulation_end.png");

			// Reset all simulation state
			uiPtr->resetCaptureArea();
			end_capture_pending = false;
			sim_init = false;
			uiPtr->sim_running = false;
			rcPtr = nullptr;
		}
		glfwPollEvents();
		glfwSwapBuffers(window);
	}
	//cleanup programs

	//glfwDestroyWindow(imgui_window);
	spherePtr->cleanup();
	linePtr->cleanup();
	halfspacePtr->cleanup();
	pointsPtr->cleanup();
	polygonPtr->cleanup();

	//cleanup glfw
	glfwTerminate();

	return 0;
}


void initGLFW(unsigned int versionMajor, unsigned int versionMinor) {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, versionMajor);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, versionMinor);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);

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
	if (plotterPtr) {
		plotterPtr->updateScreenSize(width, height);
	}
	updateCameraMatrices();
}

void processInput(double dt) {
	if (Keyboard::key(GLFW_KEY_W)) {
		camPtr->updateCameraPos(CameraDirection::FORWARD, dt);
		updateCameraMatrices();
	}
	if (Keyboard::key(GLFW_KEY_S)) {
		camPtr->updateCameraPos(CameraDirection::BACKWARD, dt);
		updateCameraMatrices();
	}
	if (Keyboard::key(GLFW_KEY_A)) {
		camPtr->updateCameraPos(CameraDirection::LEFT, dt);
		updateCameraMatrices();
	}
	if (Keyboard::key(GLFW_KEY_D)) {
		camPtr->updateCameraPos(CameraDirection::RIGHT, dt);
		updateCameraMatrices();
	}
	if (Keyboard::key(GLFW_KEY_R)) {
		camPtr->updateCameraPos(CameraDirection::UP, dt);
		updateCameraMatrices();
	}
	if (Keyboard::key(GLFW_KEY_F)&& !uiPtr->previewMode) {
		camPtr->updateCameraPos(CameraDirection::DOWN, dt);
		updateCameraMatrices();
	}

}
void updateCameraMatrices() {
	camData = camPtr->getCameraData((float) scr_width, (float)scr_height);
	//view = camPtr->getViewMatrix();
	//projection = glm::perspective(glm::radians(camPtr->getZoom()), (float)scr_width / (float)scr_height, 0.1f, 100.0f);

	view = camData.View;
	projection = camData.Projection;

	pointsPtr->updateCameraMatrices(projection * view, camPtr->cameraPos);
	polygonPtr->updateCameraMatrices(projection * view, camPtr->cameraPos);
	spherePtr-> updateCameraMatrices(projection * view, camPtr->cameraPos);
	linePtr->updateCameraMatrices(projection * view, camPtr->cameraPos);
	uiPtr->UpdateCameraMatrices(view, projection, scr_width, scr_height);
	uiPtr->cameraData = camData;
	//uiPtr->previewUp = camPtr->cameraUp;
}
void keyChanged(GLFWwindow* window, int key, int scancode, int action, int mods) {
	
	ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
	if (Keyboard::key(GLFW_KEY_ESCAPE)) {
		glfwSetWindowShouldClose(window, true);
	}
}
void cursorChanged(GLFWwindow* window, double _x, double _y) {
	ImGui_ImplGlfw_CursorPosCallback(window, _x, _y);
	double dx = Mouse::getDX();
	double dy = Mouse::getDY();
	
	if ((dx != 0 || dy != 0) && Mouse::button(GLFW_MOUSE_BUTTON_RIGHT)) {
		camPtr->updateCameraDirection(dx, dy);
	}
	
	updateCameraMatrices();	
	
}
void mouseButtonChanged(GLFWwindow* window, int button, int action, int mods) {
	ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
	mouse_x = Mouse::getMouseX();
	mouse_y = Mouse::getMouseY();
	
}
void scrollChanged(GLFWwindow* window, double dx, double dy) {
	
	double scrollDy = Mouse::getScrollDY();
	if (scrollDy != 0 && !uiPtr->previewMode) {
		camPtr->updateCameraZoom(scrollDy);
	}
	if (scrollDy != 0 && uiPtr->previewMode){
		glm::vec3 depth(camPtr->cameraFront.x + scrollDy * .1, camPtr->cameraFront.y + scrollDy * .1, camPtr->cameraFront.z + scrollDy * .1);
		uiPtr->previewDepth += glm::vec3(0.1*scrollDy)* camPtr->cameraFront; 
		camData.ScrollDepth = depth;
		// Adjust the zoom speed as needed
		//std::cout << "Preview Depth: " << uiPtr->previewDepth << std::endl;
	}
	updateCameraMatrices();
}
void characterCallback(GLFWwindow* window, unsigned int keyCode) {
	ImGui_ImplGlfw_CharCallback(window, keyCode);
	

}
glm::vec2 worldToScreen(const glm::vec3& worldPos, const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection, int screenWidth, int screenHeight) {
	// Model-View-Projection (MVP) matrix
	glm::mat4 MVP = projection * view * model;

	// Transform world coordinates to Normalized Device Coordinates (NDC)
	glm::vec4 clipSpacePos = MVP * glm::vec4(worldPos, 1.0f);
	
	glm::vec3 ndcPos = glm::vec3(clipSpacePos) / clipSpacePos.w;

	// Convert NDC to screen coordinates
	glm::vec2 screenPos;
	screenPos.x = (ndcPos.x + 1.0f) * 0.5f * screenWidth;
	screenPos.y = (1.0f - ndcPos.y) * 0.5f * screenHeight;  // Note the inverted y-axis

	return screenPos;
}
glm::vec3 screenToWorld(const glm::vec2& screenPos, float winZ, const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection, int screenWidth, int screenHeight) {
    glm::vec4 ndcPos;
    ndcPos.x = (2.0f * screenPos.x) / screenWidth - 1.0f;
    ndcPos.y = 1.0f - (2.0f * screenPos.y) / screenHeight;
    ndcPos.z = 2.0f * winZ - 1.0f; // winZ in [0,1], NDC z in [-1,1]
    ndcPos.w = 1.0f;

    glm::mat4 MVP = projection * view * model;
    glm::mat4 inverseMVP = glm::inverse(MVP);

    glm::vec4 worldPos = inverseMVP * ndcPos;
    if (worldPos.w != 0.0f) {
        worldPos /= worldPos.w;
    }
    return glm::vec3(worldPos);
}
