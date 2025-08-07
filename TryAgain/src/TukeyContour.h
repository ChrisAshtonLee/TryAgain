#pragma once
#include <iostream>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <algorithm>
#include <cmath>
#include <common/data.h>


class TukeyContour {


    bool find_median;
public:
    int max_depth;
    std::vector<Vertex> median_contour;
    TukeyContour(std::vector<Vertex> input_points, int k, bool median);
    double cross_product(Point p1, Point p2, Point p3);
    std::vector<Point> monotone_chain_convex_hull(std::vector<Point>& points);
    std::vector<Point> primal_points;
    std::vector<Lines> dual_lines;
    std::vector<Point> dual_intersections;
    std::vector<std::pair<Point, int>> intersections_with_depth;
    std::vector<DualKLevel> dual_k_levels;
    bool isAbove(Point p1, Lines p2);
    std::vector<Point> getContour(int maxdepth);
    std::vector<Point> makeUnique(std::vector<Point> arr);
    std::vector<std::pair<int,int>> dual_int_origin;
    bool validIntersection(Point l);
    bool isOn(Point p1, Lines p2);
    std::vector<DualKLevel> vert_k_levels;
};