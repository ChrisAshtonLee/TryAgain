
#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "program.h"
#include "../rendering/shader.h"
#include "../rendering/vertexmemory.hpp"
#include <vector>
#include <algorithm>

#ifndef POLYGON_HPP
#define POLYGON_HPP

class Polygon : public Program {
    int noInstances;
    std::vector<glm::vec3> points;
    std::vector<glm::vec3> colors;
    ArrayObject VAO;
    int pointCount = 0;
    Shader shader;
    glm::vec3 pdata[512];
    std::vector <unsigned int> indices;
    glm::vec3 bc;
public:
    bool addPolygon(std::vector<std::pair<float,float>> verts, float z, glm::vec3 color)  {
       
        for (int i = 0; i < verts.size(); ++i) {
            points.push_back(glm::vec3(verts[i].first, verts[i].second, z));
            colors.push_back(color);
            pointCount++;
        }
        for (int i = 0; i < points.size(); ++i) {
            std::cout << "point " << i << ": " << points[i].x << ", " << points[i].y << ", " << points[i].z << std::endl;
        }
       
        return true;
    }

    void load() {
        tessellate();
        shader = Shader(false, "polygon.vert", "polygon.frag");
        VAO.generate();
        VAO.bind();
        VAO["VBO"] = BufferObject(GL_ARRAY_BUFFER);
        VAO["VBO"].generate();
        VAO["VBO"].bind();
        

        //vector<glm::vec3> data;
       

        VAO["VBO"].setData<glm::vec3>(pointCount * 6, &pdata[0], GL_STATIC_DRAW);
        VAO["VBO"].setAttPointer<GLfloat>(0, 3, GL_FLOAT, 6, 0);
        VAO["VBO"].setAttPointer<GLfloat>(1, 3, GL_FLOAT, 6, 3);

        VAO["EBO"] = BufferObject(GL_ELEMENT_ARRAY_BUFFER);
        VAO["EBO"].generate();
        VAO["EBO"].bind();
        VAO["EBO"].setData<GLuint>((GLuint)indices.size(), &indices[0], GL_STATIC_DRAW);
    }
    void remove() {
        std::cout << "clear pressed";
        points.clear();
        indices.clear();
        pointCount = 0;
        colors.clear();
        VAO.clear();
        VAO.bind();
        VAO.generate();

    }
    void render() {
        shader.activate();
        VAO.bind();
        glDrawArrays(GL_TRIANGLES, 0, noInstances*6);
       

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
    static bool compare(const std::pair<glm::vec3, float> &a, const std::pair<glm::vec3,float> &b) {
        return a.second<b.second;
    }
    void tessellate() {
       
        std::pair<float, float> temp;
        bc = { 0,0,0 };
        for (int i = 0; i < points.size(); ++i) {
            bc.x += points[i][0];
            bc.y += points[i][1];
        }
        bc.x = bc.x / points.size();
        bc.y = bc.y / points.size();
        std::vector< std::pair<glm::vec3, float>> angles;
        for (const auto& vertex : points) {
            float theta = atan2(bc.y - vertex[1], bc.x - vertex[0]);
            angles.emplace_back(vertex, theta);
        }
        std::sort(angles.begin(), angles.end(), compare);
        for (int i = 0; i < points.size(); ++i) {
            points[i] = angles[i].first;
        }
        int j = 0;
       
        std::cout << "polygon data:" << std::endl;
        std::cout << "p" << 0 << ": " << points[0].x << points[0].y<<std::endl;
        for (int i = 1; i < points.size()-1; ++i) {
                std::cout << "p" << i << ": " << points[i].x << points[i].y << std::endl;
                pdata[j] = points[0];
                pdata[j + 1] = colors[0];
                pdata[j + 2] = points[i];
                pdata[j + 3] = colors[0];
                pdata[j + 4] = points[i + 1];
                pdata[j + 5] = colors[0];
                indices.push_back(j);
                indices.push_back(j + 1);
                indices.push_back(j + 2);
                indices.push_back(j + 3);
                indices.push_back(j + 4);
                indices.push_back(j + 5);
                j = j + 6;
                if (i == points.size() - 2) {
                    std::cout << "p" << i+1 << ": " << points[i+1].x << points[i+1].y << std::endl;
                }
                noInstances++; 
        }
        noInstances++;
    }
    
    
};
#endif