#pragma once
#include <imgui/imgui.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <src/ui/imgui_impl_glfw.h>
#include <src/imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>

#include<common/_math.h>
#include <src/geometry_primitive/halfspace.hpp>
#include <src/geometry_primitive/points.hpp>
#include <src/geometry_primitive/polygon.hpp>
#include <src/geometry_primitive/line.hpp>
#include <src/geometry_primitive/sphere.h>
#include <memory>
#include <vector>
#include <src/rendering/shader.h>
#include<scripts/ResilientConsensus.hpp>  
#include <src/TukeyContour3D.h>
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
    void DrawDemoWindow();
    void DrawWindow();
    void DrawExportWindow();
    void DrawInspectorWindow(int opt);
    void selectObjectsInBox(ImVec2 p0, ImVec2 p1);
    void UpdateCameraMatrices(glm::mat4 in_view, glm::mat4 in_proj, int in_scr_width, int in_scr_height);
    void DrawPreview( glm::vec3 pos, float radius );
    void drawSelectionBox();
    void highlightHoverSelect(ImVec2 p0);
    void SnapToPosition(glm::vec3& previewPos);
    void resetCaptureArea() {
        capture_area_set = false;
    }
    void DrawPopups();
    

    glm::vec2 UIworldToScreen( glm::vec3 worldPos,  glm::mat4 model,  glm::mat4 view,  glm::mat4 projection, int screenWidth, int screenHeight);
    glm::vec3 UIscreenToWorld(glm::vec2 screenPos, float z, glm::mat4 model,  glm::mat4 view,  glm::mat4 projection, int screenWidth, int screenHeight);
	glm::vec3 getPreviewPos();
    
    glm::vec3 vecFromString(char input[]);
    static bool sort_descend(int a, int b);
    static bool sort_ascend(int a, int b);
    const std::vector<int>& getNormalAgents() const { return normalAgents; }
    const std::vector<int>& getAdversaries() const { return adversaries; }
    bool previewInstance = false;
    bool previewMode = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    int gui_mode = 0;
    bool click_points = 0;
	bool selection = false;
    bool select_mode = false;
    bool dragging = false;
    bool shaderLoaded = false;
    bool set_capture_area_mode = false;
    bool capture_area_set = false;
    char save_path[256] = "C:/Users/85chr/source/repos/TryAgain/plots/Sim.csv";
    ImVec2 capture_start_pos;
    ImVec2 capture_end_pos;
    
   
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
    std::vector<int> normalAgents;
    std::vector<int> adversaries;

    bool sim_running = false;
private:
    void saveSimulationData(const std::string& filepath, const std::vector<std::vector<glm::vec2>>& history);
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
    std::shared_ptr<ResilientConsensus> m_rc{};
    std::unique_ptr<Quickhull3D> m_qh;
    ImDrawData * data_pointer{};
    
    
     

};



