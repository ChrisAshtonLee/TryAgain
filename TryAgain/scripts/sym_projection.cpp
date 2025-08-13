#pragma once

#include<scripts/sym_projection.h>
#include<src/TukeyContour.h>

#include <numeric>
#include <algorithm>
#include <limits>


//bool is_dividing_line(const std::vector<glm::vec2>& points, const std::pair<int, int>& line_indices) {
//    size_t n = points.size();
//    const glm::vec2& p1 = points[line_indices.first];
//    const glm::vec2& p2 = points[line_indices.second];
//
//    int positive_side = 0;
//    int negative_side = 0;
//
//    for (size_t i = 0; i < n; ++i) {
//        if (i == line_indices.first || i == line_indices.second) {
//            continue;
//        }
//
//        // Use cross-product to determine which side of the line a point is on.
//        double cross_product = (p2.x - p1.x) * (points[i].y - p1.y) -
//            (p2.y - p1.y) * (points[i].x - p1.x);
//
//        if (cross_product > 1e-9) { // Epsilon for float precision
//            positive_side++;
//        }
//        else if (cross_product < -1e-9) {
//            negative_side++;
//        }
//    }
//    
//        return positive_side == negative_side && positive_side == (n - 2) / 2;
//}
ProjectionResult TV_Projection(const std::vector<glm::vec2>& points) {
    size_t n = points.size();
    if (n == 0) {
        return { {}, {0,0} }; // Return empty result for empty input
    }

    // --- 1. Calculate the Central Point via Tukey Median Centroid ---
    glm::vec2 central_point = { 0.0f, 0.0f };

    // Convert glm::vec2 points to Vertex points for the TukeyContour class
    std::vector<Vertex> vertex_points;
    vertex_points.reserve(n);
    for (const auto& p : points) {
        vertex_points.push_back({ glm::vec3(p.x, p.y, 0.0f), glm::vec3(0.0f) });
    }

    // Calculate the Tukey Median Contour
    TukeyContour TC(vertex_points, 1, false);

    if (!TC.median_contour.empty()) {
        // Calculate the centroid of the median contour
        glm::vec3 center_sum(0.0f);
        for (const auto& v : TC.median_contour) {
            center_sum += v.position;
        }
        glm::vec3 centroid3D = center_sum / (float)TC.median_contour.size();
        central_point = glm::vec2(centroid3D.x, centroid3D.y);
    }
    else {
        std::cout << "TCMedian contour was empty." << std::endl;
        // Fallback: If no median contour, use the average of all points
        glm::vec2 sum(0.0f);
        for (const auto& p : points) {
            sum += p;
        }
        if (n > 0) {
            central_point = sum / (float)n;
        }
    }

    // --- 2. Symmetrize Points ---
    std::vector<glm::vec2> adjusted_points = points; // Make a copy to modify
    std::vector<int> points_to_pair_indices(n);
    std::iota(points_to_pair_indices.begin(), points_to_pair_indices.end(), 0); // Fills with 0, 1, 2, ...

    // If 'n' is odd, find the point closest to the center and move it there.
    if (n % 2 != 0) {
        double min_dist_sq = std::numeric_limits<double>::max();
        int closest_idx = -1;

        for (int i = 0; i < n; ++i) {
            double dist_sq = glm::length(points[i] - central_point);
            if (dist_sq < min_dist_sq) {
                min_dist_sq = dist_sq;
                closest_idx = i;
            }
        }

        // Move the closest point to the central point
        if (closest_idx != -1) {
            adjusted_points[closest_idx] = central_point;
            // Remove it from the list of points that need to be paired
            points_to_pair_indices.erase(
                std::remove(points_to_pair_indices.begin(), points_to_pair_indices.end(), closest_idx),
                points_to_pair_indices.end()
            );
        }
    }

    // --- 3. Greedily pair and reposition the remaining points ---
    std::vector<std::pair<int, int>> pairs;
    std::vector<int> unpaired_indices = points_to_pair_indices;

    while (!unpaired_indices.empty()) {
        int current_idx = unpaired_indices.front();
        unpaired_indices.erase(unpaired_indices.begin());

        if (unpaired_indices.empty()) break; // Should not happen with even numbers

        double min_dist = std::numeric_limits<double>::max();
        int partner_idx = -1;
        int partner_vec_pos = -1;

        for (size_t i = 0; i < unpaired_indices.size(); ++i) {
            double d = glm::length(points[unpaired_indices[i]] - points[current_idx]);
            if (d < min_dist) {
                min_dist = d;
                partner_idx = unpaired_indices[i];
                partner_vec_pos = i;
            }
        }

        if (partner_vec_pos != -1) {
            unpaired_indices.erase(unpaired_indices.begin() + partner_vec_pos);
            pairs.push_back({ current_idx, partner_idx });
        }
    }

    // Reposition each pair symmetrically around the central point
    for (const auto& p : pairs) {
        int idx_a = p.first;
        int idx_b = p.second;
        const glm::vec2& p_a = points[idx_a];
        const glm::vec2& p_b = points[idx_b];

        glm::vec2 vec_mid_to_a = (p_a - p_b) * 0.5f;

        adjusted_points[idx_a] = central_point + vec_mid_to_a;
        adjusted_points[idx_b] = central_point - vec_mid_to_a;
    }

    return { adjusted_points, central_point };
}