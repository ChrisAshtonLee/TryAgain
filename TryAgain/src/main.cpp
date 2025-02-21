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
#include "programs/points.hpp"
#include "programs/line.hpp"
#include "programs/polygon.hpp"
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
float mouse_x;
float mouse_y;

GLFWwindow* window = nullptr;
//GLFWwindow* imgui_window = nullptr;
//Camera
Camera cam(glm::vec3(-2.0f, 0.0f, 0.0f));
glm::mat4 view;
glm::mat4 projection;

//Programs
std::vector<Program*> programs;
std::vector<Halfspace*> spaces;
//Rectangle rect;
Sphere sphere(1);

Line line;
glm::vec3 color1 = { 1.0,0.0,0.0 };
glm::vec3 color2 = { 0.0,1.0,0.0 };
Halfspace halfspace;
Points points;
Polygon polygon;
std::vector<vector<float>> pointData;
std::vector<std::pair<float, float>> random_points;

typedef struct {
	glm::vec3 dir;
	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;
} DirLight;

int main(int, char**) {
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
	//IMGUI Window
	const char* glsl_version = "#version 130";
	
	//GLFWwindow* imgui_window = glfwCreateWindow(1280, 720, "Dear ImGui GLFW+OpenGL3 example", nullptr, nullptr);
	
	if (window == nullptr)
		return 9;
	std::vector<std::vector<float>> p = {
		{0.0,0.0},{0.0,1.0},{1.0,1.0},{1.0,0.0},{0.5,1.5},{0.5,-0.5}
	};
	
	TukeyRegion region(p, p.size());
	
	
	std::vector<std::vector<float>> k1 = region.calc();
	std::cout << "klevels[0]: ";
	for (auto e : k1[0])
		std::cout << e;
	std::cout << endl;
	std::cout << "klevels[1]: ";
	for (auto e : k1[1])
		std::cout << e << " ";
	std::cout << endl;
	std::vector<std::pair<float, float>> contour1 = region.k_contour(1);
	std::cout << " This is contour1: "<< std::endl;
	for (auto e : contour1)
		std::cout << e.first << " " << e.second << std::endl;
	std::cout << "size of klevls: " << k1.size()<<std::endl;
	

	


	//glfwMakeContextCurrent(imgui_window);
	//glfwSwapInterval(1); // Enable vsync

// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGui_ImplGlfw_InitForOpenGL(window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
	ImGui_ImplOpenGL3_Init(glsl_version);
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	
	
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	
	// Setup Platform/Renderer backends

	bool show_another_window = true;
	char input1[256] = "input1";
	char input2[256] = "input2";
	char input3[256] = "input3";
	int k_input =0;
	int n_input = 0;
	
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	
	//window params

	glfwSwapInterval(1);
	glfwSetInputMode(window, GLFW_CURSOR,GLFW_CURSOR_NORMAL);
	
	//rendering params
	glEnable(GL_DEPTH_TEST);
	glViewport(0, 0,scr_width, scr_height);
	glEnable(GL_PROGRAM_POINT_SIZE);
	
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
	//std::vector<std::pair<float, float>> polyverts = { {0.0,0.0},{1.0,0.0},{1.0,1.0},{0.0,1.0},{0.5,1.5},{0.5,-0.5} };

	//sphere.addInstance(glm::vec3(1.0f), glm::vec3(0.2f), Material::bronze);
	
	//polygon.addPolygon(polyverts, 1.0,glm::vec3(0.0,1.0,0.0));
	//glm::vec3 color2 = { 0.0,1.0,0.0 };
	//halfspace.addInstance(0.5, 0.5, 1.0, color1);
	//halfspace.print();
	//programs.push_back(&sphere);
	//points.addPoint(0.1, 0.1, 0.4, glm::vec3(1.0f, 0.0f, 0.0f));
	//points.addPoint(0.1, 0.1, 0.0, glm::vec3(0.0f, 1.0f, 0.0f)); // Green point
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
	int gui_mode = 0;
	//setup programs
	//rect.load();
	//sphere.load();
	//line.load();
	//halfspace.load();
	//points.load();
	//polygon.load();
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dis(-1.5, 1.5);
	updateCameraMatrices();

	while (!glfwWindowShouldClose(window)) {
		//update time
		
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		dt = glfwGetTime() - lastFrame;
		lastFrame += dt;
		//imgui render
		ImGui::Begin("Plotting Options");
		if (ImGui::Button("halfspaces")) {
			gui_mode = 0;
		}
		ImGui::SameLine();
		if (ImGui::Button("points")) {
			gui_mode = 1;
		}
		switch (gui_mode) {
		case 0: 
				ImGui::Text("Type in hyperplane coefficients:");
				ImGui::InputText("a1", input1, IM_ARRAYSIZE(input1));
				ImGui::InputText("a2", input2, IM_ARRAYSIZE(input2));
				ImGui::InputText("a3", input3, IM_ARRAYSIZE(input3));
				ImGui::ColorEdit3("Color", (float*)&clear_color);

				if (ImGui::Button("Color")) {
					std::cout << "color " << clear_color.w << " " << clear_color.x << " " << clear_color.z << " " << clear_color.z << std::endl;
				}
				ImGui::SameLine();
				if (ImGui::Button("Enter")) {

					halfspace.addInstance(std::atof(input1), std::atof(input2), std::atof(input3), glm::vec3(clear_color.x, clear_color.y, clear_color.z));
					halfspace.load();
					//halfspace.print();
					std::cout << atof(input1);
				}
				if (ImGui::Button("Clear Halfspaces")) {

					halfspace.clear();
				}
				break;
		case 1:
			ImGui::Text("Add Point:");
			ImGui::InputText("x", input1, IM_ARRAYSIZE(input1));
			ImGui::InputText("y", input2, IM_ARRAYSIZE(input2));
			ImGui::InputText("z", input3, IM_ARRAYSIZE(input3));
			ImGui::ColorEdit3("Color", (float*)&clear_color);
			ImGui::SameLine();
			if (ImGui::Button("Add Point")) {

				points.addPoint(std::atof(input1), std::atof(input2), std::atof(input3), glm::vec3(clear_color.x, clear_color.y, clear_color.z));
				points.load();
				vector<float> temp = { std::stof(input1), std::stof(input2) };
				pointData.push_back(temp);
				temp.clear();
			}
			if (ImGui::InputInt("k-level", &k_input)) {

			}
		
			if (ImGui::InputInt("n", &n_input))
			{ }
			if (ImGui::Button("Find k contour")) {
				
				

				TukeyRegion region2(pointData, pointData.size());
				std::vector<std::vector<float>> k2 = region2.calc();
				std::vector<std::pair<float, float>> contour1 = region2.k_contour(k_input);
				if (contour1.size() == 1) {
					std::cout << "contour is a point" << endl;
					points.addPoint(contour1[0].first, contour1[0].second, std::atof(input3), glm::vec3(clear_color.x, clear_color.y, clear_color.z));
					points.load();
					
				}
				
				if (contour1.size() == 0) {
					std::cout << "No contours for level " << k_input << std::endl;
				}
				if (contour1.size() == 2) {
				
					line.addLine(glm::vec3(contour1[0].first, contour1[0].second, std::atof(input3)), glm::vec3(contour1[1].first, contour1[1].second, std::atof(input3)));
					line.load();
				}
				if (contour1.size()>2) {
					
					std::cout << "contour" << k_input << ": " << std::endl;
					for (auto e : contour1)
						std::cout << e.first << " " << e.second << ",";
					std::cout << std::endl << "end of contour" << std::endl;
					polygon.addPolygon(contour1, atof(input3), glm::vec3(clear_color.x, clear_color.y, clear_color.z));
					polygon.load();
					}
				
			}
			if (ImGui::Button("Clear Points")) {
				polygon.remove();
				points.clear();
				
				
			}
			ImGui::SameLine();
			if (ImGui::Button("Generate n random points")) {
				
				
				for (int i = 0; i < n_input; ++i) {
					float first = dis(gen);
					float second = dis(gen);
					points.addPoint(first,second, 0.0f, glm::vec3(clear_color.x, clear_color.y, clear_color.z));
					vector<float> temp = {first,second};
					pointData.push_back(temp);
					temp.clear();

				}
				points.load();
			}
			break;
		}
		
		ImGui::End();
		
		// imgui stuff
		
		//input
		glfwWaitEventsTimeout(0.001);
		if (ImGui::GetIO().WantCaptureMouse) {
			imgui_wants_mouse = true;
			
		}
		else {
			imgui_wants_mouse = false;
			
		}
		processInput(dt);
		
		
		
		//render
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		//rect.render(dt);
		
		//sphere.render();
		line.render();
		//halfspace.render();
		points.render();
		polygon.render();
		//halfspace.print();
		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		//glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
		//glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		
		//glfwSwapBuffers(imgui_window);
		glfwPollEvents();
		glfwSwapBuffers(window);
	}
	//cleanup programs
	//rect.cleanup();
	
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	//glfwDestroyWindow(imgui_window);
	sphere.cleanup();
	line.cleanup();
	halfspace.cleanup();
	points.cleanup();
	polygon.cleanup();
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
	halfspace.updateCameraMatrices(projection * view, cam.cameraPos);
	points.updateCameraMatrices(projection * view, cam.cameraPos);
	polygon.updateCameraMatrices(projection * view, cam.cameraPos);
	
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
		cam.updateCameraDirection(dx, dy);
	}
	
	updateCameraMatrices();	
	
}
void mouseButtonChanged(GLFWwindow* window, int button, int action, int mods) {
	ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
	mouse_x = Mouse::getMouseX();
	mouse_y = Mouse::getMouseY();
	if (Mouse::button(GLFW_MOUSE_BUTTON_LEFT)) {
		glm::vec2 screenpos;
		for (int i = 0; i < points.points.size(); ++i) {

			screenpos = worldToScreen(points.points[i], glm::mat4(1.0f), view, projection, scr_width, scr_height);

			if (std::sqrt((mouse_y - screenpos[1]) * (mouse_y - screenpos[1]) + (mouse_x - screenpos[0]) * (mouse_x - screenpos[1])) < 18.0)
			{
				std::cout << "touched: " << points.points[i][0] << ", " << points.points[i][1] << endl;
			}
		}
	}
}
void scrollChanged(GLFWwindow* window, double dx, double dy) {
	
	double scrollDy = Mouse::getScrollDY();
	if (scrollDy != 0) {
		cam.updateCameraZoom(scrollDy);
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

	// Perspective division to transform to NDC
	glm::vec3 ndcPos = glm::vec3(clipSpacePos) / clipSpacePos.w;

	// Convert NDC to screen coordinates
	glm::vec2 screenPos;
	screenPos.x = (ndcPos.x + 1.0f) * 0.5f * screenWidth;
	screenPos.y = (1.0f - ndcPos.y) * 0.5f * screenHeight;  // Note the inverted y-axis

	return screenPos;
}