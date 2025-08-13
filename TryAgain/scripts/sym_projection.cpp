#pragma once

#include<scripts/sym_projection.h>





bool is_dividing_line(const std::vector<Point>& points, const std::pair<int, int>& line_indices) {
    size_t n = points.size();
    const Point& p1 = points[line_indices.first];
    const Point& p2 = points[line_indices.second];

    int positive_side = 0;
    int negative_side = 0;

    for (size_t i = 0; i < n; ++i) {
        if (i == line_indices.first || i == line_indices.second) {
            continue;
        }

        // Use cross-product to determine which side of the line a point is on.
        double cross_product = (p2.x - p1.x) * (points[i].y - p1.y) -
            (p2.y - p1.y) * (points[i].x - p1.x);

        if (cross_product > 1e-9) { // Epsilon for float precision
            positive_side++;
        }
        else if (cross_product < -1e-9) {
            negative_side++;
        }
    }

    return positive_side == negative_side && positive_side == (n - 2) / 2;
}
ProjectionResult TV_Projection(const std::vector<Point>& points) {
    size_t n = points.size();
    if (n % 2 != 0) {
        throw std::invalid_argument("The number of points (n) must be even.");
    }

    // --- 1. Find the first dividing line ---
    std::optional<std::pair<int, int>> first_line_indices;
    // This double loop is the C++ equivalent of `itertools.combinations(indices, 2)`
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = i + 1; j < n; ++j) {
            if (is_dividing_line(points, { i, j })) {
                first_line_indices = { i, j };
                goto found_first_line; // Break out of nested loops
            }
        }
    }
found_first_line:;

    if (!first_line_indices) {
        throw std::runtime_error("Could not find a line that divides the points equally.");
    }

    // --- 2. Find the second, most orthogonal dividing line ---
    std::optional<std::pair<int, int>> second_line_indices;
    double min_abs_dot_product = std::numeric_limits<double>::max();
    Point v1 = points[first_line_indices->second] - points[first_line_indices->first];

    std::vector<int> available_indices;
    for (size_t i = 0; i < n; ++i) {
        if (i != first_line_indices->first && i != first_line_indices->second) {
            available_indices.push_back(i);
        }
    }

    for (size_t i = 0; i < available_indices.size(); ++i) {
        for (size_t j = i + 1; j < available_indices.size(); ++j) {
            std::pair<int, int> p_indices = { available_indices[i], available_indices[j] };
            if (is_dividing_line(points, p_indices)) {
                Point v2 = points[p_indices.second] - points[p_indices.first];
                double abs_dot = std::abs(dot(v1, v2));

                if (abs_dot < min_abs_dot_product) {
                    min_abs_dot_product = abs_dot;
                    second_line_indices = p_indices;
                }
            }
        }
    }

    if (!second_line_indices) {
        throw std::runtime_error("Could not find a second orthogonal dividing line.");
    }

    // --- 3. Adjust points to be centrosymmetric ---
    const Point& p1 = points[first_line_indices->first];
    const Point& p2 = points[first_line_indices->second];
    const Point& p3 = points[second_line_indices->first];
    const Point& p4 = points[second_line_indices->second];

    auto intersection_opt = _find_intersection(p1, p2, p3, p4);
    if (!intersection_opt) {
        throw std::runtime_error("The two dividing lines are parallel and do not intersect.");
    }
    Point intersection_point = *intersection_opt;

    // Identify the n-4 points to be adjusted
    std::set<int> line_point_indices = {
        first_line_indices->first, first_line_indices->second,
        second_line_indices->first, second_line_indices->second
    };
    std::vector<int> points_to_adjust_indices;
    for (size_t i = 0; i < n; ++i) {
        if (line_point_indices.find(i) == line_point_indices.end()) {
            points_to_adjust_indices.push_back(i);
        }
    }

    // Greedily pair the closest points to be adjusted
    std::vector<std::pair<int, int>> pairs;
    std::vector<int> unpaired_indices = points_to_adjust_indices;

    while (!unpaired_indices.empty()) {
        int current_idx = unpaired_indices.front();
        unpaired_indices.erase(unpaired_indices.begin());

        double min_dist = std::numeric_limits<double>::max();
        int partner_idx = -1;
        int partner_vec_pos = -1;

        for (size_t i = 0; i < unpaired_indices.size(); ++i) {
            double d = norm(points[unpaired_indices[i]] - points[current_idx]);
            if (d < min_dist) {
                min_dist = d;
                partner_idx = unpaired_indices[i];
                partner_vec_pos = i;
            }
        }
        unpaired_indices.erase(unpaired_indices.begin() + partner_vec_pos);
        pairs.push_back({ current_idx, partner_idx });
    }

    // Reposition each pair symmetrically around the intersection point
    std::vector<Point> adjusted_points = points; // Make a copy
    for (const auto& p : pairs) {
        int idx_a = p.first;
        int idx_b = p.second;
        const Point& p_a = points[idx_a];
        const Point& p_b = points[idx_b];

        Point vec_mid_to_a = (p_a - p_b) / 2.0;

        adjusted_points[idx_a] = intersection_point + vec_mid_to_a;
        adjusted_points[idx_b] = intersection_point - vec_mid_to_a;
    }

    return { adjusted_points, intersection_point };
}
