#pragma once
#include <imgui/imgui.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <src/ui/imgui_impl_glfw.h>
#include <src/imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>

#include <common/data.h>
#include <src/programs/halfspace.hpp>
#include <src/programs/points.hpp>
#include <src/programs/polygon.hpp>
#include <src/programs/line.hpp>
#include <src/programs/sphere.h>
#include <memory>
#include <vector>
#include <src/rendering/shader.h>
#include <common/data.h>
#include <set>

class UI {
public:
    UI(GLFWwindow* window, const char* glsl_version, UI_DESC desc);
    ~UI();
    struct PreviewVertex {
        glm::vec3 position;
        glm::vec3 color;
    };
    struct GeomType {
        
        int POINTS = 0;
        int SPHERE = 1;
        int POLYGON = 2;
	};
    GeomType GT;
    void NewFrame();
    void Render();

    // Example drawing methods
    void DrawDemoWindow(bool* p_open);
    void DrawWindow();
    void DrawAnotherWindow(int opt);
    void selectObjectsInBox(ImVec2 p0, ImVec2 p1);
    void UpdateCameraMatrices(glm::mat4 in_view, glm::mat4 in_proj, int in_scr_width, int in_scr_height);
    void DrawPreview( glm::vec3 pos, float radius );
    void drawSelectionBox();
    void highlightHoverSelect(ImVec2 p0);
    glm::vec2 UIworldToScreen( glm::vec3 worldPos,  glm::mat4 model,  glm::mat4 view,  glm::mat4 projection, int screenWidth, int screenHeight);
    glm::vec3 UIscreenToWorld(glm::vec2 screenPos, float z, glm::mat4 model,  glm::mat4 view,  glm::mat4 projection, int screenWidth, int screenHeight);
	glm::vec3 getPreviewPos();
    
    glm::vec3 vecFromString(char input[]);
    static bool sort_descend(int a, int b);
    static bool sort_ascend(int a, int b);
    bool previewInstance = false;
    bool previewMode = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    int gui_mode = 0;
    bool click_points = 0;
	bool selection = false;
    bool select_mode = false;
    bool dragging = false;
    bool shaderLoaded = false;
   
    std::set<int> selectedSpheres;
    glm::mat4 view;
    glm::mat4 proj;
    int scr_width;
    int scr_height;
    glm::vec3  previewDepth = glm::vec3(0.0f);
	glm::vec3 previewPos;
    glm::vec3 previewRight;
    glm::vec3 previewUp;
	CameraData cameraData;
    ImGuiIO& io;
    bool pressed = false;
    ImVec4 button_default_color = ImVec4(0.2f, 0.3f, 0.4f, 1.00f);
    ImVec4 button_hover_color = ImVec4(0.0f, 0.6f, 0.2f, 1.00f);
    UI_DATA data;
    std::vector<Selection> currentSelections;
    std::vector<Selection> hoverSelections;
   
private:
    GLuint axesVAO, axesVBO;
    Shader previewShader;
    int k_input = 0;
    int n_input = 0;
    
    int maxdepth = -1;
    int preview_idx = 0;
    ImVec2 startPos;
    ImVec2 currentPos;
    ImVec2 endPos;
    std::string currentOption = " ";
    
    std::vector<std::vector<float>> pointData;
    std::vector<float> temp{};
    std::shared_ptr<Halfspace> m_halfspace{};
    std::shared_ptr<Points> m_points{};
    std::shared_ptr<Polygon> m_polygon{};
    std::shared_ptr<Line> m_line{};
    std::shared_ptr<Sphere> m_sphere{};
    
    ImDrawData * data_pointer{};
    
    
     

};



