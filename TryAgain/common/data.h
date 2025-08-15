#pragma once
#include <vector>
#include <memory>
#include <iostream>
//#include <src/geometry_primitives/points.hpp>



class Sphere;
class Points;
class Line;
class Halfspace;
class Rectangle;
class Polygon;
class GeometryPrimitive;
class ResilientConsensus;


struct Selection
{
	std::shared_ptr<GeometryPrimitive> m_geometryType;
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
	std::shared_ptr<ResilientConsensus> rc = {};
	
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
struct Point3D {
	double x, y, z;
};

struct Lines {
	double m, c;
};
struct Planes {
	double a, b, c;
};
struct DualKLevel
{
	Point point;
	int depth;
	int type;
};
struct DualKLevel3D
{
	Point3D point;
	int depth;
	int type;
};
struct DualLines {
	double m, c;
	int type;
};
struct DualPlanes {
	double a, b,c;
	int type;
};
struct ProjectionResult {
    std::vector<glm::vec2> adjusted_points;
    glm::vec2 intersection_point;
};

struct DashVertex {
	glm::vec3 position;
	glm::vec3 color;
	float line_distance;
};
struct Triangle {
	int v1, v2, v3;
};
#define M_PI  3.14159265358979323846
#define LOG(message) std::cout<<(message)<<std::endl;