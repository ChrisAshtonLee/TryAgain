#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "program.h"
#include "../rendering/shader.h"
#include "../rendering/vertexmemory.hpp"
#include <iostream>

#ifndef LINE_HPP
#define LINE_HPP

class Line : public Program {
private:
	ArrayObject VAO;
	glm::vec3 start;
	glm::vec3 end;
	glm::vec3 vertices[2];// = {
	//	{0.5f,0.5f,0.0f},
		//{0.5f,-0.5f,0.0f}
		
//	};

public:
	Line(glm::vec3 start, glm::vec3 end): start(start), end(end) {}
		void load() {
			vertices[0] = start;
			vertices[1] = end;
			
			shader = Shader(false, "line.vert", "line.frag");
			VAO.generate();
			VAO.bind();
			VAO["VBO"] = BufferObject(GL_ARRAY_BUFFER);
			VAO["VBO"].generate();
			VAO["VBO"].bind();
			VAO["VBO"].setData < glm::vec3>(2, vertices, GL_STATIC_DRAW);
			VAO["VBO"].setAttPointer<GLfloat>(0, 3, GL_FLOAT, 3, 0);

		}
		void render() {
			shader.activate();
			VAO.bind();
			VAO.draw(GL_LINES, 0, 2);
		}
		void cleanup() {
			shader.cleanup();
			VAO.cleanup();
		}
	
};
#endif