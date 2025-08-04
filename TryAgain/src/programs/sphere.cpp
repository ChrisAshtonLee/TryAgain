#include <src/programs/sphere.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>


Sphere::Sphere(unsigned int maxNoInstances)
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
void Sphere::addVertex(glm::vec3 pos, float phi, float th) {

	glm::vec2 texCoord;

	texCoord.x = th / glm::two_pi<float>();
	texCoord.y = (phi + glm::half_pi<float>()) / glm::pi<float>();

	vertices.push_back({ pos, texCoord });
};

bool Sphere::addInstance(glm::vec3 offset, glm::vec3 size, Material mat, glm::vec3 color) {
	std::cout << "addSphereInstance called." << std::endl;
	if (noInstances >= maxNoInstances) {
		return false;
	}



	offsets.push_back(offset);
	sizes.push_back(size);
	//if (color != glm::vec3(1.0f, 0.0f, 1.0f)) {
	//	mat.diffuse = color;
	//}
	diffuse.push_back(mat.diffuse);
	original_diffuse.push_back(mat.diffuse);
	specular.push_back(glm::vec4(mat.specular, mat.shininess));
	noInstances++;

	updateInstances();
	return true;
}

void Sphere::load(float r) {
	std::cout << "load called on intialization." << std::endl;
	shader = Shader(false, "sphere.vert", "dirlight.frag");
	previewShader = Shader(false, "preview.vert", "preview.frag");
	
	// generate vertices
	unsigned int res = 10; // number of rows and columns
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
	
	VAO["offsetVBO"].bind();

	VAO["offsetVBO"].setAttPointer<GLfloat>(2, 3, GL_FLOAT, 3, 0, 1);
	VAO["sizeVBO"].bind();

	VAO["sizeVBO"].setAttPointer<GLfloat>(3, 3, GL_FLOAT, 3, 0, 1);

	VAO["diffuseVBO"].bind();

	VAO["diffuseVBO"].setAttPointer<GLfloat>(4, 3, GL_FLOAT, 3, 0, 1);

	VAO["specularVBO"].bind();
	VAO["specularVBO"].setAttPointer<GLfloat>(5, 4, GL_FLOAT, 4, 0, 1);
}



void Sphere::updateInstances() {

	//std::cout << "updateInstances called." << std::endl;
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
void Sphere::render() {
	
		shader.activate();
		VAO.bind();
		VAO.draw(GL_TRIANGLES, (GLuint)indices.size(), GL_UNSIGNED_INT, 0, noInstances);
		
		GLenum err;
		while ((err = glGetError()) != GL_NO_ERROR) { std::cerr << "OpenGL Error rendering sphere: " << err << std::endl; }
	
}
void Sphere::renderPreview(const glm::vec3 pos, float radius, glm::mat4 projView) {
	

	GLenum err, err1, err2, err3;

	previewShader.activate();
	//glUseProgram(shaderProgram);
	
	float axisLen = radius * 2.0f;
	glm::vec3 color(0.0f, 1.0f, 0.0f);
	PreviewVertex verts[6] = {
	{pos + glm::vec3(-axisLen, 0, 0), color},
	{pos + glm::vec3(axisLen, 0, 0), color},
	{pos + glm::vec3(0, -axisLen, 0), color},
	{pos + glm::vec3(0, axisLen, 0), color},
	{pos + glm::vec3(0, 0, -axisLen), color},
	{pos + glm::vec3(0, 0, axisLen), color}
	};

	// 2. Draw axes using modern OpenGL (VBO/VAO)
	if (shaderLoaded == false) {
		axesVAO = 0, axesVBO = 0;
		std::cout << "renderPreview was called" << std::endl;
		if (axesVAO == 0) {

			glGenVertexArrays(1, &axesVAO);

			glGenBuffers(1, &axesVBO);
			glBindVertexArray(axesVAO);

			glBindBuffer(GL_ARRAY_BUFFER, axesVBO);

			glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);//GL_DYNAMIC_DRAW);

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(PreviewVertex), (void*)0);
			glEnableVertexAttribArray(0);

			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(PreviewVertex), (void*)(sizeof(glm::vec3)));
			glEnableVertexAttribArray(1);
			std::cout << "axesVAO and axesVBO are bound and created: " << axesVAO <<" "<<axesVBO<< std::endl;
			while ((err = glGetError()) != GL_NO_ERROR) { std::cerr << "OpenGL Error generating buffer at axesVBO: " << err << std::endl; }
		}
		shaderLoaded = true;
	}
	//GLint loc = glGetUniformLocation(shaderProgram, "proj");
	//if (loc == -1) std::cerr << "Uniform 'proj' not found in shader!" << std::endl;
	glBindVertexArray(axesVAO);
	glBindBuffer(GL_ARRAY_BUFFER, axesVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);



	//previewShader.set3Float("aColor", 0.0f, 1.0f, 0.0f);
	previewShader.setMat4("proj", projView);
	//previewShader.setVec4("FragColor", glm::vec4(color, 0.5f));
	
	glBindVertexArray(axesVAO);
	glLineWidth(2.0f);
	
	glDrawArrays(GL_LINES, 0, 6);
	
	while ((err2 = glGetError()) != GL_NO_ERROR) { std::cerr << "OpenGL Error drawing: " << err2 << std::endl; }
	glBindVertexArray(0);
}

void Sphere::cleanup() {
	shader.cleanup();
	VAO.cleanup();

	offsets.clear();
	sizes.clear();
	diffuse.clear();
	specular.clear();
}
void Sphere::deleteInstance(int i) {
	if (i < 0 || i >= noInstances) {
		std::cerr << "Invalid instance index: " << i << std::endl;
		return;
	}

	offsets.erase(offsets.begin() + i);
	sizes.erase(sizes.begin() + i);
	diffuse.erase(diffuse.begin() + i);
	original_diffuse.erase(original_diffuse.begin() + i);
	specular.erase(specular.begin() + i);
	noInstances--;
	updateInstances();

}

int Sphere::getInstanceCount() {

	return noInstances;
}
glm::vec3 Sphere::getInstanceWorldCoords(int i)
{
	return offsets[i];
}

void Sphere::highlight_selected( std::vector<int> ind) {
	
	
	if (ind.empty()) {
		
		updateInstances();
	}
	else {
		//VAO["diffuseVBO"].bind();
		for (int idx : ind) {
			//original_diffuse[idx] = diffuse[idx]; // store original color
			diffuse[idx] = glm::vec3(0.99, 0.99, 0.01); // bright yellow
			
		}
		// Update buffer with new colors
		VAO["diffuseVBO"].bind();
		VAO["diffuseVBO"].setData<glm::vec3>(noInstances, &diffuse[0], GL_STATIC_DRAW);
		updateInstances();
	}
}
// Unhighlight selected spheres by restoring their original diffuse color
void Sphere::unhighlight_selected( std::vector<int> indices) {
	VAO["diffuseVBO"].bind();
	for (int idx : indices) {
		diffuse[idx] = original_diffuse[idx]; // restore original color
	}
	// Update buffer with restored colors
	VAO["diffuseVBO"].setData<glm::vec3>(noInstances, &diffuse[0], GL_STATIC_DRAW);
}