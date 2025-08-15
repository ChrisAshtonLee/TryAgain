#pragma once
#include <iostream>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <algorithm>
#include <cmath>
#include <common/data.h>

#include <common/_math.h>
struct VertKLevel3D {
    Point3D point;
    int depth;
    int type;
    int degen_type;
};
struct ContourResult {
    std::vector<Point3D> primal_verts;
    std::vector<Triangle> triangle_indices;
};
     
class TukeyContour3D {

    bool suppress = true;
    bool find_median;
public:
    int max_depth;
    ContourResult median_contour;
    TukeyContour3D(std::vector<Vertex> input_points, int k, bool median);
    double cross_product(Point3D p1, Point3D p2, Point3D p3);
    bool validIntersection(Point3D l);
    bool isOn(Point3D p1, Planes p2);
    bool isAbove(Point3D p1, Planes p2);
    bool anyParallel(Planes p1, Planes p2, Planes p3);
    Point3D planesIntersect(Planes l1, Planes l2, Planes l3);
    Point3D planesIntersect(DualPlanes l1, DualPlanes l2, DualPlanes l3);
    ContourResult getContour(int maxdepth);
    std::vector<Point3D> makeUnique(std::vector<Point3D> arr);
    std::vector<Point3D> primal_points;
    std::vector<Planes> dual_planes;
    std::vector<Point3D> dual_intersections;
    std::vector<std::pair<Point3D, int>> intersections_with_depth;
    std::vector<DualKLevel3D> dual_k_levels;
    Quickhull3D qh;
    std::vector<Triangle> contour_indices;
    std::vector<std::pair<int, int>> dual_int_origin;
    
    std::vector<VertKLevel3D> vert_k_levels;
};