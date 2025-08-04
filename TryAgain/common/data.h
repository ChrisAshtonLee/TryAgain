#pragma once
#include <vector>
#include <memory>
//#include <src/programs/points.hpp>



class Sphere;
class Points;
class Line;
class Halfspace;
class Rectangle;
class Polygon;
class Program;

struct Selection
{
	std::shared_ptr<Program> m_geometryType;
	std::vector<int> selectedIndices;
};

struct UI_PREVIEW_DESC
{
	glm::vec3 pos = {};
	float radius = {};
	glm::vec3 color = {};
	float alpha = {};
	glm::mat4 view = {};
	glm::mat4 proj = {};
};

 struct SphereVertex {
	glm::vec3 pos;
	glm::vec2 texCoord;
} ;
 struct Vertex {
	 glm::vec3 position;
	 glm::vec3 color;
 };
struct UI_DATA 
{
	std::vector<float> coeffs = {};
	std::vector<float> points = {};
	char input1[256] = "input1";
	char input2[256] = "0.2";
	char input3[256] = "input3";
	int k_input = {};
	int n_input = {};
};
struct UI_DESC
{
	std::shared_ptr<Points> points = {};
	std::shared_ptr<Line> line = {};
	std::shared_ptr<Polygon> polygon = {};
	std::shared_ptr<Halfspace> halfspace = {};
	std::shared_ptr<Rectangle> rectangle = {};
	std::shared_ptr<Sphere> sphere = {};
	
};
struct CameraData {
	glm::mat4 Projection;
	glm::mat4 View;
	glm::vec3 CameraPos;
	glm::vec3 ScrollDepth;
	glm::vec3 Right;
	glm::vec3 Up;
	float scr_height;
	float scr_width;
};

struct Point {
	double x, y;
};


struct Lines {
	double m, c;
};