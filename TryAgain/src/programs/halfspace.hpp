#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "program.h"
#include "../rendering/shader.h"
#include "../rendering/material.h"
#include "../rendering/vertexmemory.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include <utility>

#ifndef HALFSPACE_HPP
#define HALFSPACE_HPP


class Halfspace : public Program {


	
	float scale = 0.5f;

	glm::vec3 hypervec;
	//glm::vec3 rvec;
	//glm::vec3 rvec2;
	ArrayObject VAO;
	int noInstances=0;
	int maxNoInstances = 20;
	int vertCount = 0;
	glm::vec3 vertDataMaster[96];
	std::vector < glm::vec3> colors;
	std::vector <float> coeff1;
	std::vector <float> coeff2;
	std::vector <float> zcoord;
	std::vector <unsigned int> indices;
	glm::vec3* ptrRef[12];

public:
	Halfspace() {}
	
	bool addInstance(float a1, float a2, float z, glm::vec3 inputcolor) {
		if (noInstances >= maxNoInstances) {
			return false;
		}
		
		coeff1.push_back(a1);
		coeff2.push_back(a2);
		zcoord.push_back(z);
		colors.push_back(inputcolor);
		
		noInstances++;

		glm::vec3 vertdata[12];
		
		
		glm::vec3(&ptrRef)[12] = *reinterpret_cast<glm::vec3(*)[12]>(generate_halfspace2(a1, a2, z, inputcolor));
		
		
		for (int v = 0; v < 12; ++v) {
			indices.push_back(vertCount);
			vertdata[v] = ptrRef[v];
			vertDataMaster[vertCount] = vertdata[v];
			++vertCount;
		}
		//delete[] ptrRef;
		return true;
	};
	void load() {
		
	
			

			shader = Shader(false, "halfspace.vert", "halfspace.frag");
			VAO.generate();
			VAO.bind();
			VAO["VBO"] = BufferObject(GL_ARRAY_BUFFER);
			VAO["VBO"].generate();
			VAO["VBO"].bind();
			VAO["VBO"].setData <glm::vec3>(sizeof(vertDataMaster), &vertDataMaster[0], GL_STATIC_DRAW);
		
			VAO["VBO"].setAttPointer<GLfloat>(0, 3, GL_FLOAT, 6, 0);
			VAO["VBO"].setAttPointer<GLfloat>(1, 3, GL_FLOAT, 6, 3);
		
			VAO["EBO"] = BufferObject(GL_ELEMENT_ARRAY_BUFFER);
			VAO["EBO"].generate();
			VAO["EBO"].setData<GLuint>((GLuint)indices.size(), &indices[0], GL_STATIC_DRAW);

		
	};
	void clear() {
		coeff1.clear();
		coeff2.clear();
		indices.clear();
		noInstances = 0;
		colors.clear();
	}

	void print() {
		std::cout << noInstances << std::endl;
		std::cout << "size: " << sizeof(vertDataMaster) << " contents: "<< vertDataMaster[0].x<< " "<<vertDataMaster[0].y<<" "<<vertDataMaster[0].z << std::endl;
		std::cout << "size: " << sizeof(vertDataMaster) << " contents: " << vertDataMaster[1].x << " " << vertDataMaster[1].y << " " << vertDataMaster[1].z << std::endl;
		std::cout << "indices: " << indices[0] << " " << indices[1] << std::endl;
		glm::vec3 vertdata2[12];
		
		int vertcount2 = 0;
		glm::vec3(&ptrRef2)[12] = *reinterpret_cast<glm::vec3(*)[12]>(generate_halfspace(coeff1[0], coeff2[0], zcoord[0], glm::vec3(1.0f,0.0f,0.0f)));
		
		for (int j = 0; j < noInstances * 12; ++j) {
			vertdata2[j] = ptrRef2[j];
		}
		
		for (int j = 0; j < noInstances*12; ++j) {
			//std::cout << "vertdata: " << vertdata2[j].x << " " << vertdata2[j].y << " " << vertdata2[j].z << std::endl;
			std::cout << "vertdata: " << vertdata2[j].x << " " << vertdata2[j].y << " " << vertdata2[j].z << std::endl;
		}
	};
	void render() {
		shader.activate();
		VAO.bind();
		VAO.draw(GL_TRIANGLES, 0, noInstances * 6);
	};
	void cleanup() {
		shader.cleanup();
		VAO.cleanup();
	};
	

	glm::vec3* generate_halfspace(float c1, float c2, float z, glm::vec3 instance_color) {
		// Allocate array on the heap
		glm::vec3* vert = new glm::vec3[12];

		glm::vec3 h = { c1, c2, z };
		glm::vec3 hypervec = h;  // glm::normalize(h) if needed

		glm::vec3 rvec = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::vec4{ hypervec, 1 };
		glm::vec3 rvec2 = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::vec4{ rvec, 1 };
		glm::vec3 rvec3 = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::vec4{ rvec2, 1 };

		vert[0] = glm::vec3(-scale, scale, z);
		vert[2] = glm::vec3(vert[0].x + scale * hypervec.x, vert[0].y + scale * hypervec.y, z);
		vert[4] = glm::vec3(vert[2].x + scale * rvec.x, vert[2].y + scale * rvec.y, z);
		vert[6] = vert[0];
		vert[8] = vert[4];
		vert[10] = glm::vec3(vert[4].x + scale * rvec2.x, vert[4].y + scale * rvec2.y, z);

		vert[1] = instance_color; vert[3] = instance_color; vert[5] = instance_color; vert[7] = instance_color; vert[9] = instance_color; vert[11] = instance_color;

		return vert;
	}
	glm::vec3* generate_halfspace2(float c1, float c2, float c3, glm::vec3 instance_color) {
		// Allocate array on the heap
		glm::vec3* vert = new glm::vec3[12];

		glm::vec3 h  =glm::vec3(c1,c2,c3);
		glm::vec3 p = { 1.0,0.0,0.0 };
		glm::vec3 u = glm::normalize(glm::cross(h, p));
		glm::vec3 v = glm::normalize(glm::cross(u, h));

		vert[0] = glm::vec3(u * 1.0f + v * 1.0f);
		vert[2] = glm::vec3(u*-1.0f+v*1.0f);
		vert[4] = glm::vec3(u*-1.0f-1.0f*v);
		vert[6] = vert[0];
		vert[8] = vert[4];
		vert[10] = glm::vec3(u*1.0f-1.0f*v);

		vert[1] = instance_color; vert[3] = instance_color; vert[5] = instance_color; vert[7] = instance_color; vert[9] = instance_color; vert[11] = instance_color;

		return vert;
	}
};
#endif