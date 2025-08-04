#ifndef SELECTION_BOX_HPP
#define SELECTION_BOX_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "shader.h"
#include "vertexmemory.hpp"

class SelectionBox {
private:
    Shader shader;
    ArrayObject VAO;
    BufferObject VBO;
    bool initialized = false;

public:
    SelectionBox() {
        shader = Shader(false, "selection_box.vert", "selection_box.frag");
        
        // Create a simple quad for the selection box
        float vertices[] = {
            0.0f, 0.0f,  // bottom-left
            1.0f, 0.0f,  // bottom-right
            1.0f, 1.0f,  // top-right
            0.0f, 1.0f   // top-left
        };
        
        VAO.generate();
        VAO.bind();
        
        VBO.generate();
        VBO.bind();
        VBO.setData<float>(8, vertices, GL_STATIC_DRAW);
        VBO.setAttPointer<GLfloat>(0, 2, GL_FLOAT, 2, 0);
        
        initialized = true;
    }
    
    void render(const glm::vec2& startPos, const glm::vec2& endPos, int screenWidth, int screenHeight) {
        if (!initialized) return;
        
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_DEPTH_TEST);
        
        shader.activate();
        shader.set2Float("screenSize", screenWidth, screenHeight);
        shader.set2Float("startPos", startPos.x, startPos.y);
        shader.set2Float("endPos", endPos.x, endPos.y);
        
        VAO.bind();
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
    }
    
    void cleanup() {
        if (initialized) {
            shader.cleanup();
            VAO.cleanup();
            VBO.cleanup();
        }
    }
};

#endif 