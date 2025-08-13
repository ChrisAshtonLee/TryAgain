
#include<glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <src/rendering/shader.h>
#include <common/data.h>

#include <vector>

#ifndef GEOMETRY_PRIMITIVE
#define GEOMETRY_PRIMITIVE
class GeometryPrimitive {
public:
	Shader shader;
	Shader previewShader;
	void updateCameraMatrices(glm::mat4 projView, glm::vec3 camPos);
	int noInstances =0;
	virtual void load();
	virtual void render();
	virtual void cleanup();
	virtual void highlight_selected(std::vector<int> selected) {};
	virtual void unhighlight_selected(std::vector<int> selected) {};
	virtual int getInstanceCount();
	virtual glm::vec3 getInstanceWorldCoords(int i);
	virtual void deleteInstance(int i) {};
};
#endif