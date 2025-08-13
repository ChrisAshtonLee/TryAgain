#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "program.h"
#include "../rendering/shader.h"
#include "../rendering/vertexmemory.hpp"
#include <common/data.h>
#include <iostream>
#include <vector>

#ifndef LINE_HPP
#define LINE_HPP

// A simple vertex struct for lines


class Line : public Program {
private:
    
    ArrayObject VAO;
    
    //int noInstances = 0;
    std::vector<Vertex> vertices;
    Shader shader;

public:
    
    // Constructor initializes the shader
    Line() : shader(false, "line.vert", "line.frag") {
        VAO.generate();
        VAO.bind();
        VAO["VBO"] = BufferObject(GL_ARRAY_BUFFER);
        VAO["VBO"].generate();
        VAO["VBO"].bind();
        noInstances = 0;
        // Set attribute pointers once
       // vao["VBO"].setAttPointer<GLfloat>(0, 3, GL_FLOAT, sizeof(LineVertex), (void*)offsetof(LineVertex, position));
       // vao["VBO"].setAttPointer<GLfloat>(1, 3, GL_FLOAT, sizeof(LineVertex), (void*)offsetof(LineVertex, color));
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(glm::vec3)));
        glEnableVertexAttribArray(1);
        //VAO.bind();
    }

    // Add a line segment to the list
   /* void load() {
        VAO.bind();
        VAO["VBO"].setData<Vertex>((GLuint)vertices.size(), &vertices[0], GL_STATIC_DRAW);
        VAO["VBO"].setAttPointer<GLfloat>(0, 3, GL_FLOAT, 6, 0);
        VAO["VBO"].setAttPointer<GLfloat>(1, 3, GL_FLOAT, 6, 3);
    }*/
    void addInstance(const glm::vec3& start, const glm::vec3& end, const glm::vec3& color) {
        vertices.push_back({ start, color });
        vertices.push_back({ end, color });
        noInstances++;
    }
    void updateInstances() {
        VAO.bind();
        VAO["VBO"].bind();
        // Buffer all vertex data at once
        VAO["VBO"].setData(vertices.size(), vertices.data(), GL_DYNAMIC_DRAW);
    }
    // Render all stored line segments
    void render() {
        
        if (noInstances ==0)
        {
            return;
        }
       
        shader.activate();
        GLenum err2;
        VAO.bind();
        // Draw all lines
        VAO.draw(GL_LINES, 0, vertices.size());
        while ((err2 = glGetError()) != GL_NO_ERROR) { std::cerr << "OpenGL Error drawing lines: " << err2 << std::endl; }
        VAO.bind();
        
        // Clear vertices for the next frame
        
    }
    int getInstanceCount() override {
        return noInstances;
    }
    void clear() {
        vertices.clear();
    }
    void cleanup() {
        shader.cleanup();
        VAO.cleanup();
    }
    void updateCameraMatrices(glm::mat4 projView, glm::vec3 camPos) {
        shader.activate();
        shader.setMat4("projView", projView);
        //shader.set3Float("pos", camPos);

    }
};
#endif