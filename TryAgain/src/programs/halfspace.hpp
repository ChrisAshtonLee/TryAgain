#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "program.h"
#include "../rendering/shader.h"
#include "../rendering/material.h"
#include "../rendering/vertexmemory.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#ifndef HALFSPACE_HPP
#define HALFSPACE_HPP

class Halfspace : public Program {

	float a1;
	float a2;
	float z;
	float scale = 0.5f;
	glm::vec3 vertices[12];
	glm::vec3 colour;

	glm::vec3 hypervec;
	glm::vec3 rvec;
	glm::vec3 rvec2;
	ArrayObject VAO;
	

public:
	Halfspace(float a1, float a2, float z, glm::vec3 colour) : a1(a1), a2(a2), z(z), colour(colour) {}
	void load() {
		glm::vec3 h = { a1,a2,z };

	
		
		hypervec = h;//glm::normalize(h);
		//vertices[0] = glm::vec3(-scale, scale, z);
		rvec = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::vec4{ hypervec,1 };
		rvec2 = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::vec4{ rvec,1 };
		glm::vec3 rvec3 = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) * glm::vec4{ rvec2,1 };
		vertices[0] = glm::vec3(-scale, scale, z);
		vertices[2] = glm::vec3(vertices[0].x + scale * hypervec.x,vertices[0].y + scale * hypervec.y,z);
		vertices[4] = glm::vec3(vertices[2].x + scale * rvec.x,vertices[2].y + scale * rvec.y,z);
		vertices[6] = vertices[0];//glm::vec3(-scale, scale, z);
		vertices[8] = vertices[4];//glm::vec3(vertices[1].x + scale * rvec.x, vertices[1].y + scale * rvec.y, z);
		vertices[10] = glm::vec3(vertices[4].x + scale * rvec2.x,vertices[4].y + scale * rvec2.y,z);
		std::cout << vertices[0].x <<" "<< vertices[0].y <<" "<< vertices[0].z << std::endl;
		vertices[1] = colour; vertices[3] = colour; vertices[5] = colour; vertices[7] = colour; vertices[9] = colour; vertices[11] = colour;
		
		shader = Shader(false, "halfspace.vert", "halfspace.frag");
		VAO.generate();
		VAO.bind();
		VAO["VBO"] = BufferObject(GL_ARRAY_BUFFER);
		VAO["VBO"].generate();
		VAO["VBO"].bind();
		VAO["VBO"].setData <glm::vec3> (12,vertices, GL_STATIC_DRAW);
		VAO["VBO"].setAttPointer<GLfloat>(0, 3, GL_FLOAT, 6, 0);
		VAO["VBO"].setAttPointer<GLfloat>(1, 3, GL_FLOAT, 6, 3);
	};
	void print() {
		std::cout << "vertex 0: " << vertices[0].x << " " << vertices[0].y << " " << vertices[0].z << std::endl;
		std::cout << "vertex 1: " << vertices[1].x << " " << vertices[1].y << " " << vertices[1].z << std::endl;
		std::cout << "vertex 2: " << vertices[2].x << " " << vertices[2].y << " " << vertices[2].z << std::endl;
		std::cout << "vertex 3: " << vertices[3].x << " " << vertices[3].y << " " << vertices[3].z << std::endl;
		std::cout << "vertex 4: " << vertices[4].x << " " << vertices[4].y << " " << vertices[4].z << std::endl;
		std::cout << "vertex 5: " << vertices[5].x << " " << vertices[5].y << " " << vertices[5].z << std::endl;
		std::cout << "rvec :" << rvec.x << " " << rvec.y << " " << rvec.z << std::endl;
		std::cout << "rvec2 :" << rvec2.x << " " << rvec2.y << " " << rvec2.z << std::endl;
	};
	void render() {
		shader.activate();
		VAO.bind();
		VAO.draw(GL_TRIANGLES, 0, 6);
	};
	void cleanup() {
		shader.cleanup();
		VAO.cleanup();
	};
	

};
#endif