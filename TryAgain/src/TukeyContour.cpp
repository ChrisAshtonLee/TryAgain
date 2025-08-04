#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <src/TukeyContour.h>

/**
 * @brief Calculates the cross product of three points (p1, p2, p3).
 * This helps determine the orientation of the triplet.
 * @return > 0 for a counter-clockwise turn (p3 is to the left of the vector p1->p2).
 * @return < 0 for a clockwise turn (p3 is to the right).
 * @return = 0 for collinear points.
 */

double TukeyContour::cross_product(Point p1, Point p2, Point p3) {
    return (p2.x - p1.x) * (p3.y - p1.y) - (p2.y - p1.y) * (p3.x - p1.x);
}

/**
 * @brief Computes the convex hull of a set of points using the Monotone Chain algorithm.
 * @param points A vector of points.
 * @return A vector of points representing the convex hull in counter-clockwise order.
 */
std::vector<Point> TukeyContour::monotone_chain_convex_hull(std::vector<Point>& points) {
    if (points.size() <= 2) {
        return points;
    }

    // Sort points lexicographically (by x, then by y).
    std::sort(points.begin(), points.end(), [](Point a, Point b) {
        return a.x < b.x || (a.x == b.x && a.y < b.y);
        });

    std::vector<Point> lower_hull;
    for (const auto& p : points) {
        while (lower_hull.size() >= 2 && cross_product(lower_hull[lower_hull.size() - 2], lower_hull.back(), p) <= 0) {
            lower_hull.pop_back();
        }
        lower_hull.push_back(p);
    }

    std::vector<Point> upper_hull;
    for (int i = points.size() - 1; i >= 0; --i) {
        const auto& p = points[i];
        while (upper_hull.size() >= 2 && cross_product(upper_hull[upper_hull.size() - 2], upper_hull.back(), p) <= 0) {
            upper_hull.pop_back();
        }
        upper_hull.push_back(p);
    }

    // Combine the hulls.
    lower_hull.pop_back();
    upper_hull.pop_back();
    lower_hull.insert(lower_hull.end(), upper_hull.begin(), upper_hull.end());

    return lower_hull;
}

bool TukeyContour::isAbove(Point p, Lines l)
{
    if (p.y > l.m * p.x + l.c)
    {
        return true;
    }
    else {
        return false;
    }
  
}

std::vector<Point> TukeyContour::makeUnique(std::vector<Point> arr)
{
    bool inlist; 
    std::vector<int> uniqueIdxList;
    std::vector<Point> uniqueArray;
    for (int i = 0; i < arr.size(); ++i) {
        inlist = false;
        //cout << " do this once:" << i<<" " << arr.size() << endl;
        if (uniqueIdxList.size() > 0) {
            for (int j = 0; j < uniqueIdxList.size(); ++j) {
                if (arr[i].x == arr[uniqueIdxList[j]].x && arr[i].y == arr[uniqueIdxList[j]].y) {
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


TukeyContour::TukeyContour(std::vector<Vertex> input_points, int k, bool median= false): find_median(median) {
    // --- Input Points ---
    // You can change these points to test different configurations.
   
    for (auto& p : input_points) {
        primal_points.push_back({ p.position.x, p.position.y });
	}
	float z_depth = input_points[0].position.z;
    if (primal_points.size() < 3) {
        std::cerr << "Error: At least 3 points are required." << std::endl;
        return;
    }

    std::cout << "Starting Tukey Median Contour Calculation..." << std::endl;

    // --- Step 1: Duality Transform ---
    // A point (px, py) in the primal space is transformed into a line y = px*x - py in the dual space.
   
    for (const auto& p : primal_points) {
		std::cout << "Primal Point: (" << p.x << ", " << p.y << ")" << std::endl;
        dual_lines.push_back({ p.x, -p.y });
    }
    std::cout << "Step 1: Transformed " << primal_points.size() << " points to dual lines." << std::endl;

    // --- Step 2: Find all intersection points of the dual lines ---
    
    for (size_t i = 0; i < dual_lines.size(); ++i) {
        for (size_t j = i + 1; j < dual_lines.size(); ++j) {
            const Lines& l1 = dual_lines[i];
            const Lines& l2 = dual_lines[j];
            // Check for parallel lines to avoid division by zero.
            if (std::abs(l1.m - l2.m) > 1e-9) {
                double x = (l2.c - l1.c) / (l1.m - l2.m);
                double y = l1.m * x + l1.c;
                dual_intersections.push_back({ x, y });
            }
        }
    }
    std::cout << "Step 2: Found " << dual_intersections.size() << " intersection points in the dual space." << std::endl;

    // --- Step 3: Calculate the depth of each intersection in the dual arrangement ---
    max_depth = 0;
    
    for (const auto& p : dual_intersections) {
        int lines_above = 0;
        int lines_below = 0;
        for (const auto& line : dual_lines) {
            double line_y_at_x = line.m * p.x + line.c;
            if (line_y_at_x > p.y+.0001) {
                lines_above++;
              //  std::cout << line_y_at_x << "is greater than " << p.y << std::endl;
            }
			
            if (line_y_at_x < p.y-.0001) {
				//std::cout << line_y_at_x << "is less than " << p.y << std::endl;
                lines_below++;
            }
        }
        int depth = std::min(lines_above, lines_below);
        if (depth > max_depth) {
            max_depth = depth;
        }
        if (depth == lines_above)
        {
            dual_k_levels.push_back({ p,depth, 1 });
        }
        else {
            dual_k_levels.push_back({ p,depth,-1 });
        }
       // intersections_with_depth.push_back({ p, depth });
    }
    std::cout << "Step 3: Calculated depths. Maximum depth (k*) is " << max_depth << "." << std::endl;

    // --- Step 4: Identify the vertices of the median region in the dual space ---
    std::vector<DualKLevel> median_dual_vertices;
    for (const auto& p_with_depth : dual_k_levels) {
        if (p_with_depth.depth == max_depth) {
            median_dual_vertices.push_back(p_with_depth);
            std::cout<< "  Line at (" << p_with_depth.point.x << ", " << p_with_depth.point.y 
				<< ") with depth " << p_with_depth.depth << " and type "<<p_with_depth.type<<" added to median vertices." << std::endl;
        }
    }
    std::cout << "Step 4: Identified " << median_dual_vertices.size() << " vertices with maximum depth." << std::endl;
    std::vector<DualLines> primal_contour_lines;
    if (median_dual_vertices.size() < 3) {
        //std::cerr << "Error: Degenerate case. Not enough vertices to form a contour." << std::endl;
        std::cout << "Too few vertices to form a contour." << std::endl;
        for (const auto& p : median_dual_vertices) {
            primal_contour_lines.push_back({ p.point.x, -p.point.y, p.type });
        }
        for (size_t i = 0; i < primal_contour_lines.size()-1; ++i) {
            const DualLines& l1 = primal_contour_lines[i];
            for (size_t j = i + 1; j < primal_contour_lines.size(); ++j) {
                const DualLines& l2 = primal_contour_lines[j];
                if (std::abs(l1.m - l2.m) > 1e-9) {
                    double x = (l2.c - l1.c) / (l1.m - l2.m);
                    double y = l1.m * x + l1.c;
                    median_contour.push_back({ glm::vec3(x, y,z_depth), glm::vec3(0.0f,1.0f,0.0f) });
                }
            }   
        }
        
    }
    else {
        // --- Step 5: Compute the convex hull of the median vertices in the dual space ---
       // std::vector<Point> dual_contour = monotone_chain_convex_hull(median_dual_vertices);
        std::cout << "Step 5: Computed the convex hull of the median region in the dual space." << std::endl;

        // --- Step 6: Transform the vertices of the dual contour back to primal lines ---
        // A point (dx, dy) in the dual space transforms back to a line y = dx*x - dy in the primal space.

        for (const auto& p : median_dual_vertices) {
            primal_contour_lines.push_back({ p.point.x,-p.point.y,p.type });
        }
        std::cout << "Step 6: Transformed dual contour vertices back to primal lines." << std::endl;

        // --- Step 7: Find the intersections of these primal lines to get the final contour vertices ---
        std::vector<Point> primal_vertices;
       
        for (size_t i = 0; i < primal_contour_lines.size() - 1; ++i) {
            const DualLines& l1 = primal_contour_lines[i];
            for (size_t j = i + 1; j < primal_contour_lines.size(); ++j) {
                const DualLines& l2 = primal_contour_lines[j];
                if (std::abs(l1.m - l2.m) > 1e-9) {
                    double x = (l2.c - l1.c) / (l1.m - l2.m);
                    double y = l1.m * x + l1.c;
                    Point p = { x,y };
                    bool isValid = true;
                    for (int k = 0; k < primal_contour_lines.size(); ++k)
                    {
                        Lines l = { primal_contour_lines[k].m, primal_contour_lines[k].c };
                        if (primal_contour_lines[k].type == 1) {
                            if (isAbove(p, l)) {
                                isValid = false;
                            }
                        }
                        else {
                            if (!isAbove(p, l)) {
                                isValid = false;
                            }
                        }
                    }
                    if (isValid) primal_vertices.push_back(p);
                }
            }
        }
        std::cout << "Step 7: Calculated intersection points of primal lines." << std::endl;

        // --- Step 8: The final contour is the convex hull of these primal intersection points ---
        // Sorting is necessary for display, though they should already form a convex polygon.
        std::vector<Point> final_contour = monotone_chain_convex_hull(primal_vertices);
        std::cout << "Step 8: Final contour computed." << std::endl;

        final_contour = makeUnique(final_contour);
        // --- Output the final vertices ---
        std::cout << "\n--- Tukey Median Contour Vertices ---" << std::endl;
        for (const auto& p : final_contour) {
            std::cout << "  (" << p.x << ", " << p.y << ")" << std::endl;
        }
        for (const auto& p : final_contour) {
            median_contour.push_back({ glm::vec3(p.x, p.y, z_depth), glm::vec3(0.0f, 1.0f, 1.0f) });
        }
       
    }
}
