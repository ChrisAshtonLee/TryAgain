#pragma once
#include <glm/glm.hpp>
#include <common/data.h>
#include<algorithm>
#include<optional>
#include <limits>
#include<numeric>
#include<set>
#include<utility>
//#include<cmath>
#include<vector>
#include<iostream>
#include<stdexcept>

Point operator+(const Point& a, const Point& b);
Point operator-(const Point& a, const Point& b);
Point operator/(const Point& a, double scalar);

double dot(const Point& a, const Point& b);
double norm(const Point& a);
double cross_product(Point p1, Point p2, Point p3);

std::optional<glm::vec2> _find_intersection(const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& p3, const glm::vec2& p4);

std::vector<Point> Conv2D(std::vector<Point>& points);

class Quickhull3D {
public:
    // Computes the convex hull for the given set of points.
    // Returns a vector of Triangles representing the hull faces.
    std::vector<Triangle> computeHull(const std::vector<glm::vec3>& points);
    // The set of input points
    std::vector<glm::vec3> m_points;

    // The current faces of the convex hull
    std::vector<Face> m_hullFaces;

private:
    // Internal representation of a face
    

    // Main recursive function to expand the hull
    void expandHull(Face& face);

    // Initializes the hull with the first tetrahedron
    bool initialize(const std::vector<glm::vec3>& points);

    // Calculates the signed distance from a point to a face's plane
    float distanceToFace(const Face& face, const glm::vec3& point) const;

    
};