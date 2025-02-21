
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "program.h"
#include "../rendering/shader.h"
#include "../rendering/vertexmemory.hpp"
#include <vector>

#ifndef POINTS_HPP
#define POINTS_HPP

class Points : public Program {

    //std::vector<glm::vec3> points;
    std::vector<glm::vec3> colors;
    ArrayObject VAO;
    int pointCount = 0;
    Shader shader;
    glm::vec3 pdata[256];
    std::vector <unsigned int> indices;
public:
    std::vector<glm::vec3> points;
    bool addPoint(float x, float y, float z, glm::vec3 color) {
        points.push_back(glm::vec3(x, y, z));
        colors.push_back(color);
        pointCount++;
        return true;
    }

    void load() {

        shader = Shader(false, "points.vert", "points.frag");
        VAO.generate();
        VAO.bind();
        VAO["VBO"] = BufferObject(GL_ARRAY_BUFFER);
        VAO["VBO"].generate();
        VAO["VBO"].bind();


        //vector<glm::vec3> data;
        for (int i = 0; i < pointCount; ++i) {
            //data.push_back(points[i]);
            //data.push_back(colors[i]);
            pdata[int(2 * i)] = points[i];
            pdata[int(2 * i + 1)] = colors[i];
            indices.push_back(unsigned int(2 * i));
            indices.push_back(unsigned int(2 * i + 1));
        }


        VAO["VBO"].setData<glm::vec3>(pointCount * 6, &pdata[0], GL_STATIC_DRAW);
        VAO["VBO"].setAttPointer<GLfloat>(0, 3, GL_FLOAT, 6, 0);
        VAO["VBO"].setAttPointer<GLfloat>(1, 3, GL_FLOAT, 6, 3);

        VAO["EBO"] = BufferObject(GL_ELEMENT_ARRAY_BUFFER);
        VAO["EBO"].generate();
        VAO["EBO"].bind();
        VAO["EBO"].setData<GLuint>((GLuint)indices.size(), &indices[0], GL_STATIC_DRAW);
    }
    void clear() {
        points.clear();
        indices.clear();
        pointCount = 0;
        colors.clear();
    }
    void render() {
        shader.activate();
        VAO.bind();
        glDrawArrays(GL_POINTS, 0, pointCount);

    }

    void cleanup() {
        shader.cleanup();
        VAO.cleanup();
    }
    void updateCameraMatrices(glm::mat4 projView, glm::vec3 camPos) {
        shader.activate();
        shader.setMat4("projView", projView);
        shader.set3Float("viewPos", camPos);


    }
};
#endif