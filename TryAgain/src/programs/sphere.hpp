
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <common/data.h>
#include <vector>
#include <unordered_map>

#include "program.h"
#include "../rendering/shader.h"
#include "../rendering/material.h"
#include "../rendering/vertexmemory.hpp"

#ifndef SPHERE_HPP
#define SPHERE_HPP



typedef struct {
	glm::vec3 pos;
	glm::vec2 texCoord;
} SphereVertex;

class Sphere : public Program {
	std::vector<SphereVertex> vertices;
	std::vector<unsigned int> indices;

	//unsigned int noInstances;
	unsigned int maxNoInstances;
	std::vector<glm::vec3> offsets;
	std::vector<glm::vec3> sizes;
	std::vector<glm::vec3> diffuse;
	std::vector<glm::vec3> original_diffuse;
	std::vector<glm::vec4> specular;
	glm::vec2 texCoord;
	ArrayObject VAO;
public:

	UI_PREVIEW_DESC previewDesc; 
private:
	void addVertex(glm::vec3 pos, float phi, float th) {

		glm::vec2 texCoord;

		texCoord.x = th / glm::two_pi<float>();
		texCoord.y = (phi + glm::half_pi<float>()) / glm::pi<float>();

		vertices.push_back({ pos, texCoord });
	};


public:
	
	Sphere(unsigned int maxNoInstances)
		: maxNoInstances(maxNoInstances), noInstances(0) {
		
		std::cout << "Sphere contructor started." << std::endl;
		VAO.generate();
		VAO.bind();
		VAO["VBO"] = BufferObject(GL_ARRAY_BUFFER);
		VAO["VBO"].generate();
		VAO["VBO"].bind();

		VAO["EBO"] = BufferObject(GL_ELEMENT_ARRAY_BUFFER);
		VAO["EBO"].generate();
		VAO["EBO"].bind();
		std::cout << "no Instances" << std::endl;
		
		VAO["offsetVBO"] = BufferObject(GL_ARRAY_BUFFER);
		VAO["offsetVBO"].generate();
		VAO["offsetVBO"].bind();

		VAO["sizeVBO"] = BufferObject(GL_ARRAY_BUFFER);
		VAO["sizeVBO"].generate();
		VAO["sizeVBO"].bind();

		VAO["diffuseVBO"] = BufferObject(GL_ARRAY_BUFFER);
		VAO["diffuseVBO"].generate();
		VAO["diffuseVBO"].bind();


		VAO["specularVBO"] = BufferObject(GL_ARRAY_BUFFER);
		VAO["specularVBO"].generate();
		VAO["specularVBO"].bind();
	
		std::cout << "no Instances" << std::endl;

		load(1.0f); // Load a sphere with radius 1.0
	}
	void setPreviewDesc(UI_PREVIEW_DESC desc) {
		previewDesc = desc;
	}
	bool addInstance(glm::vec3 offset, glm::vec3 size, Material mat = Material::emerald, glm::vec3 color= glm::vec3(1.0f, 0.0f, 1.0f)){
		std::cout << "addInstance called." << std::endl;
		if (noInstances >= maxNoInstances) {
			return false;
		}
	
		

		offsets.push_back(offset);
		sizes.push_back(size);
		if (color != glm::vec3(1.0f, 0.0f, 1.0f)) {
			mat.diffuse = color;
		}
		diffuse.push_back(mat.diffuse);
		original_diffuse.push_back(mat.diffuse);
		specular.push_back(glm::vec4(mat.specular, mat.shininess));
		noInstances++;
		updateInstances();
		return true;
	}
	
	void load(float r) {
		std::cout << "load called on intialization." << std::endl;
		shader = Shader(false, "sphere.vert", "dirlight.frag");
		previewShader = Shader(false, "preview.vert", "preview.frag");
		// generate vertices
		unsigned int res = 100; // number of rows and columns
		float circleStep = glm::two_pi<float>() / (float)res; // angle step between cells
		float heightStep = glm::pi<float>() / (float)res; // height of row

		int row = 0;
		int noVertices = 0;
		float phi = -glm::half_pi<float>();
		float y = glm::sin(phi);
		float radius;

		for (; phi < glm::half_pi<float>() + heightStep; phi += heightStep, row++) {
			y = r * glm::sin(phi);
			radius = r * glm::cos(phi);
			int cell = 0;
			for (float th = 0; th < glm::two_pi<float>(); th += circleStep, cell++) {
				addVertex(
					glm::vec3(radius * glm::cos(th), y, radius * glm::sin(th)),
					phi, th
				);

				// add indices if not bottom row
				if (row)
				{
					int nextCell = (cell + 1) % res;
					indices.push_back(noVertices - res); // bottom left
					indices.push_back((row - 1) * res + nextCell); // bottom right
					indices.push_back(row * res + nextCell); // top right

					indices.push_back(noVertices - res); // bottom left
					indices.push_back(noVertices); // top left (this vertex)
					indices.push_back(row * res + nextCell); // top right
				}

				noVertices++;
			}
		}
		std::cout << "vertices created." << std::endl;
		VAO.bind();
		VAO["VBO"].bind();

		VAO["VBO"].setData<SphereVertex>((GLuint)vertices.size(), &vertices[0], GL_STATIC_DRAW);
		VAO["VBO"].setAttPointer<GLfloat>(0, 3, GL_FLOAT, 5, 0);
		VAO["VBO"].setAttPointer<GLfloat>(1, 2, GL_FLOAT, 5, 3);
		VAO["EBO"].bind();
		VAO["EBO"].setData<GLuint>((GLuint)indices.size(), &indices[0], GL_STATIC_DRAW);
		std::cout << "vbo and ebo set before checking instances." << std::endl;
		
		std::cout << "there is an instance." << std::endl;
		VAO["offsetVBO"].bind();

		VAO["offsetVBO"].setAttPointer<GLfloat>(2, 3, GL_FLOAT, 3, 0, 1);
		VAO["sizeVBO"].bind();

		VAO["sizeVBO"].setAttPointer<GLfloat>(3, 3, GL_FLOAT, 3, 0, 1);

		VAO["diffuseVBO"].bind();

		VAO["diffuseVBO"].setAttPointer<GLfloat>(4, 3, GL_FLOAT, 3, 0, 1);

		VAO["specularVBO"].bind();
		VAO["specularVBO"].setAttPointer<GLfloat>(5, 4, GL_FLOAT, 4, 0, 1);
	}

	

	void updateInstances() {

		std::cout << "updateInstances called." << std::endl;
		if (noInstances == 0)
		{
			return;
		}
		VAO["offsetVBO"].bind();
		VAO["offsetVBO"].setData<glm::vec3>(noInstances, &offsets[0], GL_STATIC_DRAW);

		VAO["sizeVBO"].bind();
		VAO["sizeVBO"].setData<glm::vec3>(noInstances, &sizes[0], GL_STATIC_DRAW);

		VAO["diffuseVBO"].bind();
		VAO["diffuseVBO"].setData<glm::vec3>(noInstances, &diffuse[0], GL_STATIC_DRAW);
		

		VAO["specularVBO"].bind();
		VAO["specularVBO"].setData<glm::vec4>(noInstances, &specular[0], GL_STATIC_DRAW);
		
	}
	void render() {
		shader.activate();
		VAO.bind();
		VAO.draw(GL_TRIANGLES, (GLuint)indices.size(), GL_UNSIGNED_INT, 0, noInstances);
	}
	void renderPreview() {
		// Use a dedicated preview shader (or modify your existing one to accept color/alpha)
		//previewShader.activate();
		glm::mat4 model = glm::translate(glm::mat4(1.0f), previewDesc.pos) * glm::scale(glm::mat4(1.0f), glm::vec3(previewDesc.radius));
		previewShader.setMat4("model", model);
		previewShader.setMat4("view",previewDesc.view);
		previewShader.setMat4("projection", previewDesc.proj);
		previewShader.setVec4("color", glm::vec4(previewDesc.color, previewDesc.alpha));

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		VAO.bind();
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

		glDisable(GL_BLEND);
	}

	void cleanup() {
		shader.cleanup();
		VAO.cleanup();

		offsets.clear();
		sizes.clear();
		diffuse.clear();
		specular.clear();
	}
	void deleteInstance(int i, float r) {
		if (i < 0 || i >= noInstances) {
			std::cerr << "Invalid instance index: " << i << std::endl;
			return;
		}
		
		offsets.erase(offsets.begin() + i);
		sizes.erase(sizes.begin() + i);
		diffuse.erase(diffuse.begin() + i);
		specular.erase(specular.begin() + i);
		noInstances--;
		updateInstances();
		
	}	

	int getInstanceCount() {
		return noInstances;
	}
	glm::vec3 getInstanceWorldCoords(int i)
	{
		return offsets[i];
	}

    void highlight_selected(const std::vector<int>& indices) {
        VAO["diffuseVBO"].bind();
        for (int idx : indices) {
			original_diffuse[idx] = diffuse[idx]; // store original color
            diffuse[idx] = glm::vec3(1.0f, 1.0f, 0.0f); // bright yellow
            
        }
        // Update buffer with new colors
        VAO["diffuseVBO"].setData<glm::vec3>(noInstances, &diffuse[0], GL_STATIC_DRAW);
    }
    // Unhighlight selected spheres by restoring their original diffuse color
    void unhighlight_selected(const std::vector<int>& indices) {
        VAO["diffuseVBO"].bind();
        for (int idx : indices) {
			diffuse[idx] = original_diffuse[idx]; // restore original color
        }
        // Update buffer with restored colors
        VAO["diffuseVBO"].setData<glm::vec3>(noInstances, &diffuse[0], GL_STATIC_DRAW);
    }
};

#endif