#pragma once
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <assets/image/plotter.h>
#include <src/geometry_primitive/line.hpp>
#include <glad/glad.h>
#include <iostream>
#include <common/data.h>
#include <common/_math.h>

// Define the STB implementation in only one .cpp file

#include "stb_image_write.h"


Plotter::Plotter(std::shared_ptr<Points> in_points,std::shared_ptr<Line> in_line, int initial_width, int initial_height)
    : m_points(in_points),m_line(in_line), screen_width(initial_width), screen_height(initial_height), dash_shader(false,"dashed_line.vert", "dashed_line.frag") {

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(DashVertex), (void*)offsetof(DashVertex, position));
    glEnableVertexAttribArray(0);
    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(DashVertex), (void*)offsetof(DashVertex, color));
    glEnableVertexAttribArray(1);
    // Line distance attribute
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(DashVertex), (void*)offsetof(DashVertex, line_distance));
   // glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(DashVertex), (void*)offsetof(DashVertex, line_distance));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

Plotter::~Plotter()
{
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
}

void Plotter::updateScreenSize(int width, int height) {
    screen_width = width;
    screen_height = height;
}

void Plotter::saveFrame(const std::string& filename) {
    std::vector<unsigned char> pixels(screen_width * screen_height * 3);

    // Read pixel data from the front buffer
    glFinish();
    glReadPixels(0, 0, screen_width, screen_height, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

    // Tell stb_image_write to flip the image vertically on write
    stbi_flip_vertically_on_write(true);

    if (stbi_write_png(filename.c_str(), screen_width, screen_height, 3, pixels.data(), screen_width * 3)) {
        std::cout << "Saved frame to " << filename << std::endl;
    }
    else {
        std::cerr << "Failed to save frame to " << filename << std::endl;
    }
}
void Plotter::saveFrame(const std::string& filename, int x, int y, int width, int height) {
    if (width <= 0 || height <= 0) {
        std::cerr << "Error: Invalid capture dimensions." << std::endl;
        return;
    }
    int y_gl = screen_height - (y + height);

    std::vector<unsigned char> pixels(width * height * 3);
    glFinish();
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels( x,y_gl, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
    glPixelStorei(GL_PACK_ALIGNMENT, 4);
    stbi_flip_vertically_on_write(true);

   
    if (stbi_write_png(filename.c_str(), width, height, 3, pixels.data(), width * 3)) {
        std::cout << "Saved frame region to " << filename << std::endl;
    }
    else {
        std::cerr << "Failed to save frame region to " << filename << std::endl;
    }
    int k = 0;
    for (int i : addedIdxs) {
        m_points->deleteInstance(i - k);
        k++;
    }
    //m_line->clear();
}

void Plotter::renderTrajectories(
    const std::vector<std::vector<glm::vec2>>& history,
    const glm::mat4& projection,
    const glm::mat4& view)
{
    if (history.empty()) return;
    int idx = m_points->getInstanceCount();
    std::vector<Point> normal_points_2d;
    std::vector<DashVertex> vertices;
    int num_agents = history[0].size();
    int num_steps = history.size();
    float avg_distance = 0.0f;
    
    

    for (int agent_idx = 0; agent_idx < num_agents; ++agent_idx) {
        glm::vec3 agent_color(0.0f, 0.0f, 1.0f); // Default color
        glm::vec3 final_pos = glm::vec3(-1.8f, history.back()[agent_idx].y, history.back()[agent_idx].x);
        glm::vec3 init_pos = glm::vec3(-1.8f, history[0][agent_idx].y, history[0][agent_idx].x);
        if (agent_idx < normal_agent_size) {
            normal_points_2d.push_back(Point{ history[0][agent_idx].x, history[0][agent_idx].y });
        }
        m_points->addInstance(init_pos.x, init_pos.y, init_pos.z, agent_color);
        
        addedIdxs.push_back(idx + agent_idx);
        float x_marker_size = .0025f;
        std::vector<DashVertex> x_vertices;
        glm::vec3 p1(final_pos.x, final_pos.y - x_marker_size, final_pos.z - x_marker_size);
        glm::vec3 p2(final_pos.x, final_pos.y + x_marker_size, final_pos.z + x_marker_size);
        glm::vec3 p3(final_pos.x, final_pos.y - x_marker_size, final_pos.z + x_marker_size);
        glm::vec3 p4(final_pos.x, final_pos.y + x_marker_size, final_pos.z - x_marker_size);
        glm::vec3 x_color = glm::vec3(0.0f, 1.0f, 0.0f);
        x_vertices.push_back({ p1, x_color, 0.0f });
        x_vertices.push_back({ p2, x_color, 0.0f });
        x_vertices.push_back({ p3, x_color, 0.0f });
        x_vertices.push_back({ p4, x_color, 0.0f });
        dash_shader.activate();
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        dash_shader.setMat4("u_mvp", projection * view);
        dash_shader.setFloat("u_dashSize", 1.0f); // Make dash size large
        dash_shader.setFloat("u_gapSize", 0.0f);  // Make gap size zero for a solid line
        glBufferData(GL_ARRAY_BUFFER, x_vertices.size() * sizeof(DashVertex), x_vertices.data(), GL_DYNAMIC_DRAW);
        glDrawArrays(GL_LINES, 0, x_vertices.size());

        float  cum_distance = 0.0f;
        for (int step = 0; step < num_steps - 1; ++step) {
            glm::vec3 p1 = glm::vec3(-1.8f, history[step][agent_idx].y, history[step][agent_idx].x);
            glm::vec3 p2 = glm::vec3(-1.8f, history[step + 1][agent_idx].y, history[step + 1][agent_idx].x);

            float segment_length = glm::distance(p1, p2);
            
            // Add the start and end vertex for the line segment
            vertices.push_back({ p1, agent_color, cum_distance });
            cum_distance += segment_length;
            avg_distance += cum_distance;
            vertices.push_back({ p2, agent_color, cum_distance });
        }
    }
    std::vector<Point> hull = Conv2D(normal_points_2d);
    if (hull.size() > 1) {
        glm::vec3 hull_color(0.1f, 0.8f, 0.2f); // A nice green color for the hull
        for (size_t i = 0; i < hull.size(); ++i) {
            // Get current and next point in the hull to form a line segment
            Point p1_2d = hull[i];
            Point p2_2d = hull[(i + 1) % hull.size()]; // Wrap around for the last segment

            // Convert 2D hull points back to 3D world coordinates
            glm::vec3 p1_3d = glm::vec3(-1.8f, p1_2d.y, p1_2d.x);
            glm::vec3 p2_3d = glm::vec3(-1.8f, p2_2d.y, p2_2d.x);

            m_line->addInstance(p1_3d, p2_3d, hull_color);
        }
        // Render all hull lines in one call
       // m_line->load();
        m_line->updateInstances();
    }
    m_points->load();
    avg_distance /= (float)num_agents;
    float total_pattern_units = 10 * 2.0f;
    float dynamic_dash_size = avg_distance / (20.0f*total_pattern_units);
    float dynamic_gap_size = dynamic_dash_size;
    

    if (vertices.empty()) return;

    // --- OpenGL Drawing ---
    dash_shader.activate();
    dash_shader.setMat4("u_mvp", projection * view);
    dash_shader.setFloat("u_dashSize", dynamic_dash_size);
    dash_shader.setFloat("u_gapSize", dynamic_gap_size);
    glBindVertexArray(vao);

    // Buffer the vertex data
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(DashVertex), vertices.data(), GL_DYNAMIC_DRAW);

    // Draw the lines
    glDrawArrays(GL_LINES, 0, vertices.size());

    glBindVertexArray(0);
    
    m_points->load();
}