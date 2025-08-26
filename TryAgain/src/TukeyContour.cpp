#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <src/TukeyContour.h>
#include <fstream>
#include <iomanip>

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

bool TukeyContour::isOn(Point p, Lines l)
{
    if (std::abs(p.y - (l.m * p.x + l.c)) < 0.0001){
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
                if (std::abs(arr[i].x - arr[uniqueIdxList[j]].x)<.00001 && std::abs(arr[i].y - arr[uniqueIdxList[j]].y)<.00001) {
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

bool TukeyContour::validIntersection(Point l)
{
    bool valid = false;
    float m = l.x;
    float c =- l.y;

    for (const auto& p : primal_points) {
        
        if (std::abs(p.y - (p.x*m +c)) < .1) {
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


TukeyContour::TukeyContour(std::vector<Vertex> input_points, int k, bool median= false): find_median(median) {
    // --- Input Points ---
    // You can change these points to test different configurations.
   
    for (auto& p : input_points) {
        primal_points.push_back({ p.position.x, p.position.y });
	}
    std::string filename = "D:School/Dump/matrixdata.txt";
    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        std::cerr << "Could not open file " << filename << std::endl;
    }
    outFile << std::fixed << std::setprecision(6);  
    for (const auto& point : primal_points) {
        outFile << point.x << " " << point.y << "\n";
    }
    outFile.close();
	float z_depth = input_points[0].position.z;
    if (primal_points.size() < 3) {
        std::cerr << "Error: At least 3 points are required." << std::endl;
        return;
    }

    if (!suppress) std::cout << "Starting Tukey Median Contour Calculation..." << std::endl;

    // --- Step 1: Duality Transform ---
    // A point (px, py) in the primal space is transformed into a line y = px*x - py in the dual space.
   
    for (const auto& p : primal_points) {
		if (!suppress) std::cout << "Primal Point: (" << p.x << ", " << p.y << ")" << std::endl;
        dual_lines.push_back({ p.x, -p.y });
    }
 //   std::cout << "Step 1: Transformed " << primal_points.size() << " points to dual lines." << std::endl;

    // --- Step 2: Find all intersection points of the dual lines ---
    
    for (int i = 0; i < dual_lines.size(); ++i) {
        for (int j = i + 1; j < dual_lines.size(); ++j) {
            const Lines& l1 = dual_lines[i];
            const Lines& l2 = dual_lines[j];
            // Check for parallel lines to avoid division by zero.
          
            if (std::abs(l1.m - l2.m) > 0.00000001f) {
                double x = (l2.c - l1.c) / (l1.m - l2.m);
                double y = l1.m * x + l1.c;
                if (validIntersection(Point{ x,y })) {
                    dual_intersections.push_back({ x, y });
                }
            }
            else {
               // std::cout << "vertical line encountered." << std::endl;
               // LOG("vertical line encountered.");
                float xPos = primal_points[j].x;
                int left = 0; int right = 0;
                for (int pp = 0; pp < primal_points.size(); ++pp) {
                    if (primal_points[pp].x < xPos && std::abs(xPos-primal_points[pp].x)>.000000001)
                    {
                        left++;
                    }
                    if (primal_points[pp].x > xPos && std::abs(xPos - primal_points[pp].x)>.000000001)
                    {
                        right++;
                    }
                }
                if (right > left) {
                    vert_k_levels.push_back({ Point{xPos,0.0f},left+1,1 });
                    std::cout << "vertical line: " << "x = " << xPos << " type 1" << " depth: " << left + 1 << std::endl;
                }
                if (left > right) {
                    vert_k_levels.push_back({ Point{xPos,0.0f},right+1,-1 });
                    std::cout << "vertical line: " << "x = " << xPos << " type -1" << " depth: " << right + 1 << std::endl;
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
        for (const auto& line : dual_lines) {
            double line_y_at_x = line.m * p.x + line.c;
            if (line_y_at_x > p.y+.00001) {
                lines_above++;
              //  std::cout << line_y_at_x << "is greater than " << p.y << std::endl;
            }
			
            if (line_y_at_x < p.y-.00001) {
				//std::cout << line_y_at_x << "is less than " << p.y << std::endl;
                lines_below++;
            }
        }
        int depth = std::min(lines_above, lines_below);
        if (depth +1> max_depth) {
            max_depth = depth+1;
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
    std::vector<Point> final_contour = getContour(max_depth);
    while (final_contour.size() == 0)
    {
        median_contour.clear();
        max_depth--; 
        std::cout << "contour with depth " << max_depth + 1 << " is empty, trying depth " << max_depth << std::endl;
        
        if (max_depth < 0) {
            std::cout << "no contour found." << std::endl;
            break;
        }
        final_contour = getContour(max_depth);
        
        
    }
        // --- Output the final vertices ---
     //   std::cout << "\n--- Tukey Median Contour Vertices ---" << std::endl;
    if (!suppress) {
        for (const auto& p : final_contour) {
            std::cout << "  (" << p.x << ", " << p.y << ")" << std::endl;
        }
    }
        for (const auto& p : final_contour) {
            median_contour.push_back({ glm::vec3(p.x, p.y, z_depth), glm::vec3(0.0f, 1.0f, 1.0f) });
        }
       
    }



std::vector<Point> TukeyContour::getContour(int k)
{
    std::vector<DualKLevel> median_dual_vertices;
    std::vector<int> boundary_indices;
    std::vector<DualLines> lower_depth_lines;
    bool upper = false; bool lower = false;
    int boundary_count = 0;
    for (int m =0; m<dual_k_levels.size(); ++m){
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
            lower_depth_lines.push_back(DualLines{ p_with_depth.point.x,-p_with_depth.point.y, p_with_depth.type });
        }

    }
    if (!upper || !lower) {
        if (!suppress) std::cout << "boundary incomplete." << std::endl;
        std::vector<Point> emptylist;
        if (!suppress) std::cout << "size of emptylist: " << emptylist.size() << std::endl;
        return emptylist;

    }
  //  std::cout << "Step 4: Identified " << median_dual_vertices.size() << " vertices with maximum depth." << std::endl;
    std::vector<DualLines> primal_contour_lines;
   
    if (median_dual_vertices.size() < 3) {
        //std::cerr << "Error: Degenerate case. Not enough vertices to form a contour." << std::endl;
        std::cout << "Too few vertices to form a contour." << std::endl;
        for (const auto& p : median_dual_vertices) {
            primal_contour_lines.push_back({ p.point.x, -p.point.y, p.type });
           
        }
        for (size_t i = 0; i < primal_contour_lines.size() - 1; ++i) {
            const DualLines& l1 = primal_contour_lines[i];
            for (size_t j = i + 1; j < primal_contour_lines.size(); ++j) {
                const DualLines& l2 = primal_contour_lines[j];
                if (std::abs(l1.m - l2.m) > 1e-9) {
                    double x = (l2.c - l1.c) / (l1.m - l2.m);
                    double y = l1.m * x + l1.c;
                    median_contour.push_back({ glm::vec3(x, y,0.0f), glm::vec3(0.0f,1.0f,0.0f) });
                }
            }
        }

    }
    else {
        // --- Step 5: Compute the convex hull of the median vertices in the dual space ---
       // std::vector<Point> dual_contour = monotone_chain_convex_hull(median_dual_vertices);
    //    std::cout << "Step 5: Computed the convex hull of the median region in the dual space." << std::endl;

        // --- Step 6: Transform the vertices of the dual contour back to primal lines ---
        // A point (dx, dy) in the dual space transforms back to a line y = dx*x - dy in the primal space.
        
        for (const auto& p : median_dual_vertices) {
             
            primal_contour_lines.push_back({ p.point.x,-p.point.y,p.type });
         //  std::cout << "primal contour line m: " << p.point.x << " c: " << -p.point.y << std::endl;
        }
      
     
      //  std::cout << "Step 6: Transformed dual contour vertices back to primal lines." << std::endl;
       
        // --- Step 7: Find the intersections of these primal lines to get the final contour vertices ---
        std::vector<Point> primal_vertices;

        for (int i = 0; i < primal_contour_lines.size() - 1; ++i) {
            const DualLines& l1 = primal_contour_lines[i];
            for (int j = i + 1; j < primal_contour_lines.size(); ++j) {
                const DualLines& l2 = primal_contour_lines[j];
                if (std::abs(l1.m - l2.m) > 1e-9) {
                    double x = (l2.c - l1.c) / (l1.m - l2.m);
                    double y = l1.m * x + l1.c;
                   // std::cout << "Candidate point " << x << ", " << y << " found." << std::endl;
                    Point p = { x,y };
                    bool isValid = true;
                    for (int v = 0; v < boundary_indices.size(); ++v)
                    {
                       
                        Lines l = { primal_contour_lines[boundary_indices[v]].m, primal_contour_lines[boundary_indices[v]].c };
                        if (primal_contour_lines[boundary_indices[v]].type == 1) {
                            if (!isAbove(p, l) && std::abs(l.m) < 1000000000) {//&&!isOn(p,l)) {
                                //  && boundary_indices[k] != i && boundary_indices[k] != j
                                isValid = false;
                                if (max_depth == 5) {
                                std::cout << "FAIL: Point " << p.x << " " << p.y << " is below line m: " << l.m << " c: " << l.c << std::endl;
                            }
                            }
                           /* else {
                                std::cout << "PASS: Point " << p.x << " " << p.y << " is above line m: " << l.m << " c: " << l.c << std::endl;
                            }*/

                        }
                        if (primal_contour_lines[boundary_indices[v]].type == -1){
                            if (isAbove(p, l) && std::abs(l.m) < 1000000000){// && !isOn(p,l)) {
                                // && boundary_indices[k] != i && boundary_indices[k] != j
                                isValid = false;
                                if (max_depth == 5) {
                                    std::cout << "FAIL: Point " << p.x << " " << p.y << " is above line m: " << l.m << " c: " << l.c << std::endl;
                                }
                           }
                          /*  else {
                                std::cout << "PASS: Point " << p.x << " " << p.y << " is below line m: " << l.m << " c: " << l.c << std::endl;
                            }*/

                        }
                   
                    }
                    for (int r = 0; r < lower_depth_lines.size(); ++r)
                    {
                        DualLines low = lower_depth_lines[r];
                        Lines l = Lines{ low.m,low.c };
                        if (low.type == -1) {
                            if (isAbove(p, l) && std::abs(l.m) < 1000000000 ){// && !isOn(p, l)) {
                                isValid = false;
                            }
                        }
                        if (low.type == 1) {
                            if (!isAbove(p, l) && std::abs(l.m) < 1000000000 ){//&& !isOn(p, l)) {
                                isValid = false;
                        
                            }
                        }
                    
                    }
                    if (vert_k_levels.size() > 0) {
                        for (int vp = 0; vp < vert_k_levels.size(); ++vp) {
                            DualKLevel vertline = vert_k_levels[vp];
                            if (vertline.depth <= k) {
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
                        }
                    }
                    if (isValid)
                        {
                            primal_vertices.push_back(p);
                        }
                    }
            }
        }
     //   std::cout << "Step 7: Calculated intersection points of primal lines." << std::endl;

        // --- Step 8: The final contour is the convex hull of these primal intersection points ---
        // Sorting is necessary for display, though they should already form a convex polygon.
        std::vector<Point> final_contour = monotone_chain_convex_hull(primal_vertices);
     //   std::cout << "Step 8: Final contour computed." << std::endl;
      
        final_contour = makeUnique(final_contour);
        if (!suppress) {
            std::cout << "unique points of final contour: " << std::endl;
            for (auto& s : final_contour)
            {
                std::cout << s.x << " , " << s.y << " " << std::endl;
            }
        }
        return final_contour;
    }
}