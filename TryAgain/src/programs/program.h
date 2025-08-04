
#include<glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <src/rendering/shader.h>
#include <common/data.h>

#include <vector>

#ifndef PROGRAM_H
#define PROGRAM_H
class Program {
public:
	Shader shader;
	Shader previewShader;
	void updateCameraMatrices(glm::mat4 projView, glm::vec3 camPos);

	virtual void load();
	virtual void render(double dt);
	virtual void cleanup();
	virtual void highlight_selected(std::vector<int> selected) {};
	virtual void unhighlight_selected(std::vector<int> selected) {};
	virtual int getInstanceCount();
	virtual glm::vec3 getInstanceWorldCoords(int i);
	virtual void deleteInstance(int i) {};
};
#endif