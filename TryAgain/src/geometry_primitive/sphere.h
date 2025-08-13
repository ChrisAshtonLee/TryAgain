#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <common/data.h>
#include <vector>
#include <unordered_map>

#include <src/geometry_primitive/geometry_primitive.h>
#include "../rendering/shader.h"
#include "../rendering/material.h"
#include "../rendering/vertexmemory.hpp"
extern const char* vertexShaderSource;
extern const char* fragmentShaderSource;

class Sphere : public GeometryPrimitive {
	std::vector<SphereVertex> vertices;
	std::vector<unsigned int> indices;

	unsigned int noInstances;
	unsigned int maxNoInstances;
	
	
	glm::vec2 texCoord;
	ArrayObject VAO;

	
private:
	void addVertex(glm::vec3 pos, float phi, float th);
	GLuint axesVAO = 0, axesVBO = 0;
public:
	Sphere(unsigned int maxNoInstances);
	std::vector<glm::vec3> offsets;
	std::vector<glm::vec3> sizes;
	std::vector<glm::vec3> diffuse;
	std::vector<glm::vec3> original_diffuse;
	std::vector<glm::vec4> specular;

	GLuint vertexShader;
	GLuint fragmentShader;
	GLuint shaderProgram;

	bool shaderLoaded = false;
	bool preview = false;
	struct PreviewVertex {
		glm::vec3 position;
		glm::vec3 color;
	};

	bool addInstance(glm::vec3 offset, glm::vec3 size, Material mat = Material::emerald, glm::vec3 color = glm::vec3(1.0f, 0.0f, 1.0f));
	void load(float r);
	void updateInstances();
	void render();
	void renderPreview(glm::vec3 pos, float radius, glm::mat4 projview);
	void cleanup();
	void deleteInstance(int i);
	int getInstanceCount();
	glm::vec3 getInstanceWorldCoords(int i);
	void highlight_selected( std::vector<int> indices);
	void unhighlight_selected(std::vector<int> indices);

};