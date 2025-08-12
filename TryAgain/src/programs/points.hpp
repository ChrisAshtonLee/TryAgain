
#include <common/data.h>
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
    
    Shader shader;
   
    std::vector <unsigned int> indices;
    
  
public:
    std::vector<glm::vec3> original_colors;
    int noInstances = 0;
    std::vector<Vertex> points;
    
    Points(): shader(false, "points.vert", "plotpoints.frag")
    {
       
        std::vector<glm::vec3> sizes;
       // shader = Shader(false, "points.vert", "points.frag");
        VAO.generate(); // Generate VAO ID
        VAO.bind();     // Bind VAO

        // Initialize VBO within the VAO
        VAO["VBO"] = BufferObject(GL_ARRAY_BUFFER);
        VAO["VBO"].generate(); // Generate VBO ID
        VAO["VBO"].bind();     // Bind VBO
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(glm::vec3)));
        glEnableVertexAttribArray(1);

       /* VAO["VBO"].setAttPointer<Vertex>(0, 3, GL_FLOAT, sizeof(Vertex), 0);
        VAO["VBO"].setAttPointer<Vertex>(1, 3, GL_FLOAT, sizeof(Vertex), sizeof(glm::vec3));*/

        /*VAO["VBO"].setAttPointer<Vertex>(0, 3, GL_FLOAT, 6, 0);
        VAO["VBO"].setAttPointer<Vertex>(1, 3, GL_FLOAT, 6, 3);*/



       
        // Unbind VAO to prevent accidental modification
        VAO.bind(); // Rebind VAO after setup for clarity, then unbind
       // glBindVertexArray(0); // Unbind VAO
      //  glBindBuffer(GL_ARRAY_BUFFER, 0); // Unbind VBO
        
    }


    bool addInstance(float x, float y, float z, glm::vec3 color)
    {
        if (noInstances >= 256) {
            std::cerr << "Maximum number of points reached." << std::endl;
            return false;
        }
        points.push_back({ glm::vec3(x, y, z), color });
		original_colors.push_back(color); // Store original color for highlighting
        noInstances++;
        //std::cout << "point added: " <<points[noInstances-1].position.x<<" "<< points[noInstances - 1].position.y << " " << points[noInstances - 1].position.z << " " << std::endl;
        
		return true;
    }
    void deleteInstance(int idx) {
        if (idx < 0 || idx >= noInstances) {
            std::cerr << "Invalid index: " << idx << std::endl;
            return;
        }
        points.erase(points.begin() + idx);
		original_colors.erase(original_colors.begin() + idx); // Remove corresponding original color
        noInstances--;
        //std::cout << "point deleted." << std::endl;
	}
    void load() {

        VAO.bind();
        VAO["VBO"].bind();


        //vector<glm::vec3> data;
 

       // VAO["VBO"].setData<Vertex>((GLuint)points.size(), &points[0], GL_STATIC_DRAW);

        
        
       // VAO["VBO"].setAttPointer<Vertex>(0, 3, GL_FLOAT, sizeof(Vertex), 0);
       // VAO["VBO"].setAttPointer<Vertex>(1, 3, GL_FLOAT, sizeof(Vertex), sizeof(glm::vec3));

        VAO["VBO"].setData<Vertex>((GLuint)noInstances, &points[0], GL_DYNAMIC_DRAW);
        //VAO["VBO"].setAttPointer<Vertex>(0, 3, GL_FLOAT, sizeof(Vertex), 0);
        //VAO["VBO"].setAttPointer<Vertex>(1, 3, GL_FLOAT, sizeof(Vertex), sizeof(glm::vec3));
       


    }
  
    void clear() {
        points.clear();
        indices.clear();
        noInstances = 0;
        colors.clear();
    }
    void render() {
        shader.activate();
        VAO.bind();
        GLenum err2;
		VAO.draw(GL_POINTS, 0, noInstances);
       
        //glDrawArrays(GL_POINTS, 0, noInstances);
        while ((err2 = glGetError()) != GL_NO_ERROR) { std::cerr << "OpenGL Error drawing: " << err2 << std::endl; }
        // glDrawArrays(GL_POINTS, 0, noInstances);

    }
  
    void highlight_selected(std::vector<int> indices) {
        //VAO.bind();
        for (int idx : indices) {
           
            points[idx].color = glm::vec3(1.0f, 1.0f, 0.0f); // bright yellow
			
        }
        // Update buffer with new colors
        load();
        // VAO["VBO"].setData<Vertex>((GLuint)noInstances, &points[0], GL_STATIC_DRAW);
    }

    void unhighlight_selected(std::vector<int> indices)  {
        //VAO["VBO"].bind();
		
        for (int idx : indices) {
            points[idx].color = original_colors[idx]; // restore original color
        }
        // Update buffer with restored colors
        //VAO["VBO"].setData<Vertex>((GLuint)noInstances, &points[0], GL_STATIC_DRAW);
        load();
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
    int getInstanceCount() override {
        return noInstances;
	}
    std::vector<glm::vec3> getAllWorldCoords() {
        std::vector<glm::vec3> coords;
        for (int i = 0; i < noInstances; i++) {
            coords.push_back(points[i].position);
		}
        return coords;
    }
    glm::vec3 getInstanceWorldCoords(int i) override {
        if (i < 0 || i >= noInstances) {
            std::cerr << "Invalid instance index: " << i << std::endl;
            return glm::vec3(0.0f);
        }
        return points[i].position;
	}
};
#endif