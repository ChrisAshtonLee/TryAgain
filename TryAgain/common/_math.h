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