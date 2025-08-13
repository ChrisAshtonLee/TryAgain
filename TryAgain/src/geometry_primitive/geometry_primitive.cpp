#include "geometry_primitive.h"

void GeometryPrimitive::updateCameraMatrices(glm::mat4 projView, glm::vec3 camPos) {
	shader.activate();
	
	shader.setMat4("projView", projView);
	shader.set3Float("viewPos", camPos);
	//previewShader.setMat4("proj", projView);
	//previewShader.set3Float("aPos", camPos);

}
void GeometryPrimitive::load() {}
void GeometryPrimitive::render() {}
void GeometryPrimitive::cleanup() {}

int GeometryPrimitive::getInstanceCount() {
	// Your implementation code here
	// For example:
	return 0;
}

// Definition for the getInstanceWorldCoords function
glm::vec3 GeometryPrimitive::getInstanceWorldCoords(int i) {
	// Your implementation code here
	// For example:
	return glm::vec3(0.0f);
}