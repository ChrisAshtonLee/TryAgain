#include <src/TukeyContour3D.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>

#include <fstream>
#include <iomanip>

/**
 * @brief Calculates the cross product of three points (p1, p2, p3).
 * This helps determine the orientation of the triplet.
 * @return > 0 for a counter-clockwise turn (p3 is to the left of the vector p1->p2).
 * @return < 0 for a clockwise turn (p3 is to the right).
 * @return = 0 for collinear points.
 */

double TukeyContour3D::cross_product(Point3D p1, Point3D p2, Point3D p3) {
    return (p2.x - p1.x) * (p3.y - p1.y) - (p2.y - p1.y) * (p3.x - p1.x);
}

/**
 * @brief Computes the convex hull of a set of points using the Monotone Chain algorithm.
 * @param points A vector of points.
 * @return A vector of points representing the convex hull in counter-clockwise order.
 */

bool TukeyContour3D::isAbove(Point3D p, Planes l)
{
    if (p.z > l.a * p.x + l.b*p.y+l.c)
    {
        return true;
    }
    else {
        return false;
    }
}

bool TukeyContour3D::isOn(Point3D p, Planes l)
{
    if (std::abs(p.z - (l.a * p.x +l.b*p.z+ l.c)) < 0.0001) {
        return true;
    }
    else {
        return false;
    }

}


std::vector<Point3D> TukeyContour3D::makeUnique(std::vector<Point3D> arr)
{
    bool inlist;
    std::vector<int> uniqueIdxList;
    std::vector<Point3D> uniqueArray;
    for (int i = 0; i < arr.size(); ++i) {
        inlist = false;
        //cout << " do this once:" << i<<" " << arr.size() << endl;
        if (uniqueIdxList.size() > 0) {
            for (int j = 0; j < uniqueIdxList.size(); ++j) {
                if (std::abs(arr[i].x - arr[uniqueIdxList[j]].x) < .00001 && std::abs(arr[i].y - arr[uniqueIdxList[j]].y) < .00001
                    && std::abs(arr[i].z - arr[uniqueIdxList[j]].z) < .00001) {
                    inlist = true;
                }
            }
            if (!inlist) {
                uniqueIdxList.push_back(i);
            }
        }
        else {
            uniqueIdxList.push_back(i);
        }
    }
    for (int k = 0; k < uniqueIdxList.size(); ++k) {
        uniqueArray.push_back(arr[uniqueIdxList[k]]);
    }
    return uniqueArray;
}
bool TukeyContour3D::anyParallel(Planes p1, Planes p2, Planes p3)
{
    bool res = false;
    if ((p1.a == p2.a && p1.b == p2.b) || (p1.a == p3.a && p1.a == p3.b) || (p2.a == p3.a && p2.b == p3.b)) { res = true; }
    return res;
}

Point3D TukeyContour3D::planesIntersect(Planes l1, Planes l2, Planes l3)
{

    if (!anyParallel(l1, l2, l3)) {
        glm::vec4 p1(-l1.a, -l1.b, 1.0f, -l1.c);
        glm::vec4 p2(-l2.a, -l2.b, 1.0f, -l2.c);
        glm::vec4 p3(-l3.a, -l3.b, 1.0f, -l3.c);
        glm::mat3 A = glm::mat3(
            glm::vec3(p1.x, p1.y, p1.z),
            glm::vec3(p2.x, p2.y, p2.z),
            glm::vec3(p3.x, p3.y, p3.z)
        );
        glm::vec3 B(-p1.w, -p2.w, -p3.w);
        float detA = glm::determinant(A);
        if (std::abs(detA) > 1e-6)
        {
            glm::mat3 A_x = A;
            A_x[0] = B;
            glm::mat3 A_y = A;
            A_y[1] = B;
            glm::mat3 A_z = A;
            A_z[2] = B; // Replace third column (for z)
            Point3D cand_point = Point3D({ glm::determinant(A_x) / detA, glm::determinant(A_y) / detA, glm::determinant(A_z) / detA });
            return cand_point;
        }
        
    }
}
Point3D TukeyContour3D::planesIntersect(DualPlanes d1, DualPlanes d2, DualPlanes d3)
{
    Planes l1 = Planes({ d1.a, d1.b,d1.c }); Planes l2 = Planes({ d2.a, d2.b,d2.c }); Planes l3 = Planes({ d3.a, d3.b,d3.c });
    if (!anyParallel(l1, l2, l3)) {
        glm::vec4 p1(-l1.a, -l1.b, 1.0f, -l1.c);
        glm::vec4 p2(-l2.a, -l2.b, 1.0f, -l2.c);
        glm::vec4 p3(-l3.a, -l3.b, 1.0f, -l3.c);
        glm::mat3 A = glm::mat3(
            glm::vec3(p1.x, p2.x, p3.x),
            glm::vec3(p1.y, p2.y, p3.y),
            glm::vec3(p1.z, p2.z, p3.z)
        );
        glm::vec3 B(-p1.w, -p2.w, -p3.w);
        float detA = glm::determinant(A);
        if (std::abs(detA) > 1e-6)
        {
            glm::mat3 A_x = A;
            A_x[0] = B;
            glm::mat3 A_y = A;
            A_y[1] = B;
            glm::mat3 A_z = A;
            A_z[2] = B; // Replace third column (for z)
            Point3D cand_point = Point3D({ glm::determinant(A_x) / detA, glm::determinant(A_y) / detA, glm::determinant(A_z) / detA });
            return cand_point;
        }

    }
}
bool TukeyContour3D::validIntersection(Point3D l)
{
    bool valid = false;
    float a = l.x;
    float b = l.y;
    float c = -l.z;
    for (const auto& p : primal_points) {

        if (std::abs(p.z - (p.x * a +p.y*b+ c)) < .1) {
            valid = true;

        }
        /*  else {
              std::cout << "p.y: " << p.y << " p.x: " << p.x << " m:" << m << " c:" << c << " diff: " << std::abs(p.y - (p.x * m + c)) << std::endl;
          }*/

    }
    /*if (!valid) {

        std::cout << "Dual Line: " << l.x << ", " << l.y << " is not valid." << std::endl;
    }*/
    return valid;
}


TukeyContour3D::TukeyContour3D(std::vector<Vertex> input_points, int k, bool median = false) : find_median(median) {
    // --- Input Points ---
    // You can change these points to test different configurations.

    for (auto& p : input_points) {
        primal_points.push_back({ p.position.x, p.position.y,p.position.z });
    }
    std::string filename = "D:School/Dump/matrixdata.txt";
    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        std::cerr << "Could not open file " << filename << std::endl;
    }
    outFile << std::fixed << std::setprecision(6);
    for (const auto& point : primal_points) {
        outFile << point.x << " " << point.y <<" "<<point.z << "\n";
    }
    outFile.close();
    float z_depth = input_points[0].position.z;
    if (primal_points.size() < 4) {
        std::cerr << "Error: At least 3 points are required." << std::endl;
        return;
    }

    if (!suppress) std::cout << "Starting Tukey Median Contour Calculation..." << std::endl;

    // --- Step 1: Duality Transform ---
    // A point (px, py) in the primal space is transformed into a line y = px*x - py in the dual space.

    for (const auto& p : primal_points) {
        if (!suppress) std::cout << "Primal Point3D: (" << p.x << ", " << p.y << ")" << std::endl;
        dual_planes.push_back({ p.x,p.y, -p.z });
    }
    //   std::cout << "Step 1: Transformed " << primal_points.size() << " points to dual lines." << std::endl;

       // --- Step 2: Find all intersection points of the dual lines ---

    for (int i = 0; i < dual_planes.size(); ++i) {
        for (int j = i + 1; j < dual_planes.size(); ++j) {
            for (int k = j+1; k<dual_planes.size(); ++k){
                const Planes& l1 = dual_planes[i];
                const Planes& l2 = dual_planes[j];
                const Planes& l3 = dual_planes[k];
                // Check for parallel lines to avoid division by zero.

                if (!anyParallel(l1,l2,l3)) {
                    glm::vec4 p1(-l1.a, -l1.b, 1.0f, -l1.c);
                    glm::vec4 p2(-l2.a, -l2.b, 1.0f, -l2.c);
                    glm::vec4 p3(-l3.a, -l3.b, 1.0f, -l3.c);
                    glm::mat3 A = glm::mat3(
                        glm::vec3(p1.x, p2.x, p3.x),
                        glm::vec3(p1.y, p2.y, p3.y),
                        glm::vec3(p1.z, p2.z, p3.z)
                    );
                    glm::vec3 B(-p1.w, -p2.w, -p3.w);
                    float detA = glm::determinant(A);
                    if (std::abs(detA) > 1e-6)
                    {
                        glm::mat3 A_x = A;
                        A_x[0] = B; 
                        glm::mat3 A_y = A;
                        A_y[1] = B; 
                        glm::mat3 A_z = A;
                        A_z[2] = B; // Replace third column (for z)
                        Point3D cand_point = Point3D({ glm::determinant(A_x) / detA, glm::determinant(A_y) / detA, glm::determinant(A_z) / detA });
                        std::cout << "planes: " << "l1: " << l1.a << " " << l1.b << " " << l1.c << std::endl;
                        std::cout << "l2: " << l2.a << " " << l2.b << " " << l2.c << std::endl;
                        std::cout << "l3: " << l3.a << " " << l3.b << " " << l3.c << std::endl;
                        std::cout << "intersect at point :" << cand_point.x << " " << cand_point.y << " " << cand_point.z << std::endl;
                        dual_intersections.push_back(cand_point);
                       
                      /*  if (validIntersection(cand_point)) {
                            dual_intersections.push_back(cand_point);
                        }*/
                    }
                    else {
                        if (primal_points[i].x < 1e-6 && primal_points[j].x < 1e-6 && primal_points[k].x < 1e-6)
                        {
                            // all points lie on line y = c
                            int above = 0, below = 0;
                            float yPos = primal_points[i].y;
                            for (int pp = 0; pp < primal_points.size(); ++pp) {
                                if (primal_points[pp].y < yPos && std::abs(yPos - primal_points[pp].y)>.000000001)
                                {
                                    below++;
                                }
                                if (primal_points[pp].y > yPos && std::abs(yPos - primal_points[pp].y) > .000000001)
                                {
                                    above++;
                                }
                            }
                            if (above > below) {
                                vert_k_levels.push_back({ Point3D{yPos,0.0f,0.0f},below + 1,1,2 });
                                std::cout << "horizontal line: " << "y = " << yPos << " type 1" << " depth: " << below + 1 << std::endl;
                            }
                            if (below > above) {
                                vert_k_levels.push_back({ Point3D{0.0f,yPos,0.0f},above + 1,-1,2 });
                                std::cout << "horizontal line: " << "y = " << yPos << " type -1" << " depth: " << above + 1 << std::endl;
                            }
                        }
                        else if (primal_points[i].y < 1e-6 && primal_points[j].y < 1e-6 && primal_points[k].y < 1e-6)
                        {
                            //all points lie on line x = c
                            int left = 0, right = 0;
                            float xPos = primal_points[i].x;
                            for (int pp = 0; pp < primal_points.size(); ++pp) {
                                if (primal_points[pp].x < xPos && std::abs(xPos - primal_points[pp].x)>.000000001)
                                {
                                    left++;
                                }
                                if (primal_points[pp].x > xPos && std::abs(xPos - primal_points[pp].x) > .000000001)
                                {
                                    right++;
                                }
                            }
                            if (right > left) {
                                vert_k_levels.push_back({ Point3D{xPos,0.0f,0.0f},left + 1,1 ,1});
                                std::cout << "vertical line: " << "x = " << xPos << " type 1" << " depth: " << left + 1 << std::endl;
                            }
                            if (left > right) {
                                vert_k_levels.push_back({ Point3D{xPos,0.0f,0.0f},right + 1,-1,1 });
                                std::cout << "vertical line: " << "x = " << xPos << " type -1" << " depth: " << right + 1 << std::endl;
                            }
                        }
                        else
                        {
                            float m = (primal_points[i].y - primal_points[j].y) / (primal_points[i].x - primal_points[j].x);
                            float b = primal_points[i].y - m * primal_points[i].x;
                            int above = 0, below = 0;
                            for (int pp = 0; pp < primal_points.size(); ++pp) {
                                if (primal_points[pp].y < primal_points[pp].x * m + b && std::abs(primal_points[pp].y - primal_points[pp].x * m + b)>.0000001) {
                                    below++;
                                }
                                else if (primal_points[pp].y > primal_points[pp].x * m + b && std::abs(primal_points[pp].y - primal_points[pp].x * m + b)>.0000001) {
                                    above++;
                                }
                            }
                            if (above > below) {
                                vert_k_levels.push_back({ Point3D{m,b,0.0f},below + 1,1,3});
                                //std::cout << "horizontal line: " << "y = " << yPos << " type 1" << " depth: " << below + 1 << std::endl;
                            }
                            if (below > above) {
                                vert_k_levels.push_back({ Point3D{m,b,0.0f},above + 1,-1,3 });
                                //std::cout << "horizontal line: " << "y = " << yPos << " type -1" << " depth: " << above + 1 << std::endl;
                            }
                            //points lie on line ax+by = c
                        }
                        // std::cout << "vertical line encountered." << std::endl;
                        // LOG("vertical line encountered.");
                       
                    }
                 }
                    
            }
        }
    }
    //  std::cout << "Step 2: Found " << dual_intersections.size() << " intersection points in the dual space." << std::endl;

      // --- Step 3: Calculate the depth of each intersection in the dual arrangement ---
    max_depth = 0;

    for (const auto& p : dual_intersections) {
        int lines_above = 0;
        int lines_below = 0;
        for (const auto& line : dual_planes) {
            double line_z_at_x_y = line.a * p.x + line.b*p.y+p.z;
            //std::cout << "dual intersection: " << p.x << " " << p.y << " " << p.z << std::endl;
          // std::cout << "dual_plane: " << line.a << " " << line.b << " " << line.c << std::endl;
            if (line_z_at_x_y > p.z + .00001) {
                lines_above++;
                std::cout << line_z_at_x_y << "is greater than " << p.z << std::endl;
            }

            if (line_z_at_x_y < p.z - .00001) {
                std::cout << line_z_at_x_y << "is less than " << p.z << std::endl;
                lines_below++;
            }
        }
        int depth = std::min(lines_above, lines_below);
        if (depth + 1 > max_depth) {
            max_depth = depth + 1;
        }
        if (lines_above == lines_below) {
            dual_k_levels.push_back({ p,depth + 1,0 });
        }
        else {
            if (depth == lines_above)
            {
                dual_k_levels.push_back({ p,depth + 1, 1 });
            }
            else {
                dual_k_levels.push_back({ p,depth + 1,-1 });
            }
        }
        // intersections_with_depth.push_back({ p, depth });
    }
    std::cout << "Step 3: Calculated depths. Maximum depth (k*) is " << max_depth << "." << std::endl;

      // --- Step 4: Identify the vertices of the median region in the dual space ---
    ContourResult final_contour = getContour(max_depth);
   
    while (final_contour.primal_verts.size() == 0)
    {
        median_contour.primal_verts.clear();
        median_contour.triangle_indices.clear();
        max_depth--;
        std::cout << "contour with depth " << max_depth + 1 << " is empty, trying depth " << max_depth << std::endl;
        if (max_depth < 0) {
            std::cout << "no contour found." << std::endl;
            break;
        }
        final_contour = getContour(max_depth);


    }
    // --- Output the final vertices ---
    std::cout << "\n--- Tukey Median Contour Vertices ---" << std::endl;
    if (suppress) {
        for (const auto& p : final_contour.primal_verts) {
            std::cout << "  (" << p.x << ", " << p.y << ","<< p.z<< ")" << std::endl;
        }
    }
    median_contour = final_contour;

}



ContourResult TukeyContour3D::getContour(int k)
{
    std::vector<DualKLevel3D> median_dual_vertices;
    std::vector<int> boundary_indices;
    std::vector<DualPlanes> lower_depth_planes;
    bool upper = false; bool lower = false;
    int boundary_count = 0;
    for (int m = 0; m < dual_k_levels.size(); ++m) {
        const auto& p_with_depth = dual_k_levels[m];
        if (p_with_depth.depth == k) {
            // std::cout << "point " << p_with_depth.point.x << " " << p_with_depth.point.y << " with depth " << p_with_depth.depth << " added to boundary." << std::endl;
            boundary_indices.push_back(boundary_count);
        }
        if (p_with_depth.depth >= k) {
            median_dual_vertices.push_back(p_with_depth);
            boundary_count++;
            /*  std::cout << "  Line at (" << p_with_depth.point.x << ", " << p_with_depth.point.y
                  << ") with depth " << p_with_depth.depth << " and type " << p_with_depth.type << " added to median vertices." << std::endl;*/
            if (p_with_depth.type == 1) {
                upper = true;
            }
            else {
                lower = true;
            }
        }

        if (p_with_depth.depth < k) {
            lower_depth_planes.push_back(DualPlanes{ p_with_depth.point.x,p_with_depth.point.y,-p_with_depth.point.z, p_with_depth.type });
        }

    }
    if (!upper || !lower) {
        if (!suppress) std::cout << "boundary incomplete." << std::endl;
        ContourResult emptylist;
        if (!suppress) std::cout << "size of emptylist: " << emptylist.primal_verts.size() << std::endl;
        return emptylist;

    }
    //  std::cout << "Step 4: Identified " << median_dual_vertices.size() << " vertices with maximum depth." << std::endl;
    std::vector<DualPlanes> primal_contour_planes;

    if (median_dual_vertices.size() < 4) {
        //std::cerr << "Error: Degenerate case. Not enough vertices to form a contour." << std::endl;
        std::cout << "Too few vertices to form a contour." << std::endl;
        //for (const auto& p : median_dual_vertices) {
        //    primal_contour_planes.push_back({ p.point.x, -p.point.y, p.type });

        //}
        //for (size_t i = 0; i < primal_contour_planes.size() - 1; ++i) {
        //    const DualPlanes& l1 = primal_contour_planes[i];
        //    for (size_t j = i + 1; j < primal_contour_planes.size(); ++j) {
        //        const DualPlanes& l2 = primal_contour_planes[j];
        //        if (std::abs(l1.m - l2.m) > 1e-9) {
        //            double x = (l2.c - l1.c) / (l1.m - l2.m);
        //            double y = l1.m * x + l1.c;
        //            // median_contour.push_back({ glm::vec3(x, y,0.0f), glm::vec3(0.0f,1.0f,0.0f) });
        //        }
        //    }
        //}

    }
    else {
        // --- Step 5: Compute the convex hull of the median vertices in the dual space ---
       // std::vector<Point3D> dual_contour = monotone_chain_convex_hull(median_dual_vertices);
    //    std::cout << "Step 5: Computed the convex hull of the median region in the dual space." << std::endl;

        // --- Step 6: Transform the vertices of the dual contour back to primal lines ---
        // A point (dx, dy) in the dual space transforms back to a line y = dx*x - dy in the primal space.

        for (const auto& p : median_dual_vertices) {

            primal_contour_planes.push_back({ p.point.x,p.point.y,-p.point.z,p.type });
            //  std::cout << "primal contour line m: " << p.point.x << " c: " << -p.point.y << std::endl;
        }


        //  std::cout << "Step 6: Transformed dual contour vertices back to primal lines." << std::endl;

          // --- Step 7: Find the intersections of these primal lines to get the final contour vertices ---
        std::vector<Point3D> primal_vertices;

        for (int i = 0; i < primal_contour_planes.size() - 2; ++i) {
            const DualPlanes& l1 = primal_contour_planes[i];
            for (int j = i + 1; j < primal_contour_planes.size() - 1; ++j) {
                const DualPlanes& l2 = primal_contour_planes[j];
                for (int k = j + 1; k < primal_contour_planes.size(); ++k)
                {
                    const DualPlanes& l3 = primal_contour_planes[k];
                    Planes d1 = Planes({ l1.a, l1.b,l1.c }); Planes d2 = Planes({ l2.a, l2.b,l2.c }); Planes d3 = Planes({ l3.a, l3.b,l3.c });
                    if (!anyParallel(d1, d2, d3)) {

                        
                        Point3D p = planesIntersect(l1, l2, l3);
                        std::cout << "Candidate point " << p.x << ", " << p.y <<", "<< p.z << " found." << std::endl;

                        bool isValid = true;
                        for (int v = 0; v < boundary_indices.size(); ++v)
                        {

                            Planes l = { primal_contour_planes[boundary_indices[v]].a, primal_contour_planes[boundary_indices[v]].b,primal_contour_planes[boundary_indices[v]].c };
                            if (primal_contour_planes[boundary_indices[v]].type == 1) {
                                if (!isAbove(p, l) && std::abs(l.a) < 1000000000 && std::abs(l.b) < 10000000000 && !isOn(p, l)) {
                                    //  && boundary_indices[k] != i && boundary_indices[k] != j
                                    isValid = false;
                                    // std::cout << "FAIL: Point3D " << p.x << " " << p.y << " is below line m: " << l.m << " c: " << l.c << std::endl;
                                }
                                /* else {
                                     std::cout << "PASS: Point3D " << p.x << " " << p.y << " is above line m: " << l.m << " c: " << l.c << std::endl;
                                 }*/

                            }
                            if (primal_contour_planes[boundary_indices[v]].type == -1) {
                                if (isAbove(p, l) && std::abs(l.a) < 1000000000 && std::abs(l.b) < 1000000000 && !isOn(p, l)) {
                                    // && boundary_indices[k] != i && boundary_indices[k] != j
                                    isValid = false;
                                    // std::cout << "FAIL: Point3D " << p.x << " " << p.y << " is above line m: " << l.m << " c: " << l.c << std::endl;
                                }
                                /*  else {
                                      std::cout << "PASS: Point3D " << p.x << " " << p.y << " is below line m: " << l.m << " c: " << l.c << std::endl;
                                  }*/

                            }

                        }
                        for (int r = 0; r < lower_depth_planes.size(); ++r)
                        {
                            DualPlanes low = lower_depth_planes[r];
                            Planes l = Planes{ low.a,low.b,low.c };
                            if (low.type == -1) {
                                if (isAbove(p, l) && std::abs(l.a) < 1000000000 && std::abs(l.b) < 1000000000 && !isOn(p, l)) {
                                    isValid = false;
                                }
                            }
                            if (low.type == 1) {
                                if (!isAbove(p, l) && std::abs(l.a) < 1000000000 && std::abs(l.b) < 1000000000 && !isOn(p, l)) {
                                    isValid = false;

                                }
                            }

                        }
                        if (vert_k_levels.size() > 0) {
                            for (int vp = 0; vp < vert_k_levels.size(); ++vp) {
                                VertKLevel3D vertline = vert_k_levels[vp];
                                if (vertline.depth <= k) {
                                    if (vertline.degen_type == 1) {
                                        if (vertline.type == 1) {
                                            if (p.x < vertline.point.x)
                                            {
                                                isValid = false;
                                            }
                                        }
                                        if (vertline.type == -1) {
                                            if (p.x > vertline.point.x)
                                            {
                                                isValid = false;
                                            }
                                        }
                                    }
                                    if (vertline.degen_type == 2) {
                                        if (vertline.type == 1) {
                                            if (p.y < vertline.point.y) {
                                                isValid = false;
                                            }
                                        }
                                        if (vertline.type == -1) {
                                            if (p.y > vertline.point.y) {
                                                isValid = false;
                                            }
                                        }
                                    }
                                    if (vertline.degen_type == 3) {
                                        if (vertline.type == 1) {
                                            if (p.y < vertline.point.x * p.x + vertline.point.y) {
                                                isValid = false;
                                            }
                                        }
                                        if (vertline.type == -1) {
                                            if (p.y > vertline.point.x * p.x + vertline.point.y) {
                                                isValid = false;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        if (isValid)
                        {
                            primal_vertices.push_back(p);
                        }
                    }
                }

            }
        }
        //   std::cout << "Step 7: Calculated intersection points of primal lines." << std::endl;

           // --- Step 8: The final contour is the convex hull of these primal intersection points ---
           // Sorting is necessary for display, though they should already form a convex polygon.
        std::vector<glm::vec3> primal_vertices_glm;
        primal_vertices = makeUnique(primal_vertices);
        for (Point3D v : primal_vertices) {
            primal_vertices_glm.push_back(glm::vec3(v.x, v.y, v.z));
        }
        if (primal_vertices_glm.size() > 3) {
            contour_indices = qh.computeHull(primal_vertices_glm);
        }
       
        std::vector<Point3D> final_contour;

        //   std::cout << "Step 8: Final contour computed." << std::endl;

        //final_contour = makeUnique(final_contour);
        if (!suppress) {
            std::cout << "unique points of final contour: " << std::endl;
            for (auto& s : final_contour)
            {
                std::cout << s.x << " , " << s.y << " " << std::endl;
            }
        }
        ContourResult res = ContourResult({ primal_vertices, contour_indices});
        return res;
    }
}