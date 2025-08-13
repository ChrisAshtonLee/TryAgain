#pragma once
#include <glm/glm.hpp>
#include <common/data.h>
#include <common/_math.h>
#include <string>
#include <vector>
#include <memory>

#include <src/rendering/shader.h>
#include<src/geometry_primitive/points.hpp>
#include <src/geometry_primitive/line.hpp>

// Forward-declare the Line class to avoid including its full header here


class Plotter {
public:
    // Constructor to initialize with screen dimensions
    Plotter(std::shared_ptr<Points> in_points,std::shared_ptr<Line> in_line,int initial_width, int initial_height);
    ~Plotter();
    // Updates the stored screen dimensions if the window is resized
    void updateScreenSize(int width, int height);

    // Captures the current framebuffer and saves it to a PNG file
    void saveFrame(const std::string& filename);
    void saveFrame(const std::string& filename, int x, int y, int width, int height);
    // Renders agent trajectories from history as dashed lines
    void renderTrajectories(
        const std::vector<std::vector<glm::vec2>>& history,
        const glm::mat4& projection,
        const glm::mat4& view);
    std::shared_ptr<Points> m_points;
    std::shared_ptr<Line> m_line;
    std::vector<int>addedIdxs;
    int normal_agent_size=0;
private:
    int screen_width;
    int screen_height;
    Shader dash_shader;
    unsigned int vao;
    unsigned int vbo;
   
};