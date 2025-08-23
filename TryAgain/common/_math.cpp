#include <common/_math.h>
#include <algorithm>
#include <stdexcept>
#include <map>
#include <set>

Point operator+(const Point& a, const Point& b) { return { a.x + b.x, a.y + b.y }; }
Point operator-(const Point& a, const Point& b) { return { a.x - b.x, a.y - b.y }; }
Point operator/(const Point& a, double scalar) { return { a.x / scalar, a.y / scalar }; }


double dot(const Point& a, const Point& b) { return a.x * b.x + a.y * b.y; }
double norm(const Point& a) { return std::sqrt(a.x * a.x + a.y * a.y); }
double cross_product(Point p1, Point p2, Point p3) {
    return (p2.x - p1.x) * (p3.y - p1.y) - (p2.y - p1.y) * (p3.x - p1.x);
}
std::optional<glm::vec2> _find_intersection(const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& p3, const glm::vec2& p4) {
    
     // Line 1 (p1, p2): A1x + B1y = C1
     double a1 = p2.y - p1.y;
     double b1 = p1.x - p2.x;
     double c1 = a1 * p1.x + b1 * p1.y;

     // Line 2 (p3, p4): A2x + B2y = C2
     double a2 = p4.y - p3.y;
     double b2 = p3.x - p4.x;
     double c2 = a2 * p3.x + b2 * p3.y;

     double determinant = a1 * b2 - a2 * b1;

     // Use a small epsilon for floating-point comparison.
     if (std::abs(determinant) < 1e-9) {
         return std::nullopt; // Lines are parallel
     }

     glm::vec2 intersection;
     intersection.x = (b2 * c1 - b1 * c2) / determinant;
     intersection.y = (a1 * c2 - a2 * c1) / determinant;

     return intersection;
}

std::vector<Point> Conv2D(std::vector<Point>& points) {
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





std::vector<Triangle> Quickhull3D::computeHull(const std::vector<glm::vec3>& points) {
    if (points.size() < 4) {
        throw std::runtime_error("Quickhull requires at least 4 points.");
    }

    m_points = points;
    m_hullFaces.clear();

    if (!initialize(points)) {
        throw std::runtime_error("Could not initialize the hull. Points may be degenerate (collinear/coplanar).");
    }

    // Process all faces that have points on their outside
    for (size_t i = 0; i < m_hullFaces.size(); ++i) {
        // A face might be removed during expansion, so check if it's still valid
        if (i < m_hullFaces.size() && !m_hullFaces[i].outsidePoints.empty()) {
            expandHull(m_hullFaces[i]);
        }
    }

    // Convert internal Face format to the public Triangle format
    std::vector<Triangle> final_hull;
    for (const auto& face : m_hullFaces) {
        final_hull.push_back({ face.v1, face.v2, face.v3 });
    }
    return final_hull;
}

bool Quickhull3D::initialize(const std::vector<glm::vec3>& points) {
    const float epsilon = 1e-5f;

    // 1. Find two most distant points to form the first edge.
    int idx1 = 0, idx2 = 0;
    float max_dist_sq = 0.0f;

    for (size_t i = 0; i < points.size(); ++i) {
        for (size_t j = i + 1; j < points.size(); ++j) {
            glm::vec3 diff = points[j] - points[i];
            float d_sq = glm::dot(diff,diff);
            if (d_sq > max_dist_sq) {
                max_dist_sq = d_sq;
                idx1 = i;
                idx2 = j;
            }
        }
    }

    if (max_dist_sq < epsilon * epsilon) return false; // All points are coincident.

    // 2. Find the point farthest from the line segment (idx1, idx2).
    int idx3 = -1;
    float max_dist_from_line = 0.0f;
    glm::vec3 line_dir = glm::normalize(points[idx2] - points[idx1]);
    for (size_t i = 0; i < points.size(); ++i) {
        if (i == idx1 || i == idx2) continue;
        float dist = glm::length(glm::cross(points[i] - points[idx1], line_dir));
        if (dist > max_dist_from_line) {
            max_dist_from_line = dist;
            idx3 = i;
        }
    }

    if (idx3 == -1 || max_dist_from_line < epsilon) return false; // All points are collinear.

    // 4. Find the point farthest from the plane defined by (idx1, idx2, idx3).
    int idx4 = -1;
    float max_dist_from_plane = 0.0f;
    glm::vec3 plane_normal = glm::normalize(glm::cross(points[idx2] - points[idx1], points[idx3] - points[idx1]));
    for (size_t i = 0; i < points.size(); ++i) {
        if (i == idx1 || i == idx2 || i == idx3) continue;
        float dist = std::abs(glm::dot(points[i] - points[idx1], plane_normal));
        if (dist > max_dist_from_plane) {
            max_dist_from_plane = dist;
            idx4 = i;
        }
    }

    if (idx4 == -1 || max_dist_from_plane < epsilon) return false; // All points are coplanar.

    // 5. We now have a non-degenerate tetrahedron (idx1, idx2, idx3, idx4).
    // Ensure the fourth point is on the positive side of the plane (idx1, idx2, idx3)
    // to maintain a consistent winding order (counter-clockwise).
    if (glm::dot(plane_normal, points[idx4] - points[idx1]) < 0) {
        // If it's on the negative side, swap two vertices to flip the normal.
        std::swap(idx2, idx3);
    }

    // 6. Create the initial four faces of the tetrahedron.
    int v1 = idx1, v2 = idx2, v3 = idx3, v4 = idx4;
    m_hullFaces.emplace_back(v1, v2, v3, glm::normalize(glm::cross(m_points[v2] - m_points[v1], m_points[v3] - m_points[v1])), 0);
    m_hullFaces.emplace_back(v1, v3, v4, glm::normalize(glm::cross(m_points[v3] - m_points[v1], m_points[v4] - m_points[v1])), 0);
    m_hullFaces.emplace_back(v1, v4, v2, glm::normalize(glm::cross(m_points[v4] - m_points[v1], m_points[v2] - m_points[v1])), 0);
    m_hullFaces.emplace_back(v2, v4, v3, glm::normalize(glm::cross(m_points[v4] - m_points[v2], m_points[v3] - m_points[v2])), 0);

    // 7. Correct normals to point outwards from the tetrahedron's centroid.
    glm::vec3 centroid = (m_points[v1] + m_points[v2] + m_points[v3] + m_points[v4]) / 4.0f;
    for (auto& face : m_hullFaces) {
        if (glm::dot(face.normal, m_points[face.v1] - centroid) < 0) {
            face.normal = -face.normal;
            std::swap(face.v2, face.v3); // Maintain CCW order
        }
        face.distanceToOrigin = glm::dot(face.normal, m_points[face.v1]);
    }

    // 8. Assign all other points to the outside set of the first face they are outside of.
    std::set<int> hull_points = { v1, v2, v3, v4 };
    for (size_t i = 0; i < points.size(); ++i) {
        if (hull_points.count(i)) continue;

        for (auto& face : m_hullFaces) {
            if (distanceToFace(face, points[i]) > epsilon) {
                face.outsidePoints.push_back(i);
                break; // Assign to the first face it's outside of
            }
        }
    }
    return true;
}


void Quickhull3D::expandHull(Face& face) {
    if (face.outsidePoints.empty() || face.isVisible) {
        return;
    }

    // 1. Find the farthest point from the face
    int farthest_point_idx = -1;
    float max_dist = 0.0f;
    for (int point_idx : face.outsidePoints) {
        float dist = distanceToFace(face, m_points[point_idx]);
        if (dist > max_dist) {
            max_dist = dist;
            farthest_point_idx = point_idx;
        }
    }
    if (farthest_point_idx == -1) return;

    // 2. Identify all visible faces from the farthest point
    std::vector<Face*> visible_faces;
    std::map<std::pair<int, int>, int> edge_counts;

    for (auto& f : m_hullFaces) {
        if (distanceToFace(f, m_points[farthest_point_idx]) > 1e-5f) {
            f.isVisible = true;
            visible_faces.push_back(&f);

            auto add_edge = [&](int a, int b) {
                if (a > b) std::swap(a, b);
                edge_counts[{a, b}]++;
                };
            add_edge(f.v1, f.v2);
            add_edge(f.v2, f.v3);
            add_edge(f.v3, f.v1);
        }
    }

    // 3. Find the horizon edges
    std::vector<Edge> horizon_edges;
    for (const auto& pair : edge_counts) {
        if (pair.second == 1) {
            horizon_edges.push_back({ pair.first.first, pair.first.second });
        }
    }

    // 4. Collect all points from the outside sets of the visible faces
    std::vector<int> points_to_reassign;
    for (auto* f_ptr : visible_faces) {
        points_to_reassign.insert(points_to_reassign.end(), f_ptr->outsidePoints.begin(), f_ptr->outsidePoints.end());
    }

    // 5. Remove visible faces from the hull
    m_hullFaces.erase(std::remove_if(m_hullFaces.begin(), m_hullFaces.end(),
        [](const Face& f) { return f.isVisible; }), m_hullFaces.end());

    // 6. Create new faces from the horizon edges to the farthest point
    size_t new_faces_start_index = m_hullFaces.size();
    for (const auto& edge : horizon_edges) {
        glm::vec3 normal = glm::normalize(glm::cross(m_points[edge.v2] - m_points[edge.v1], m_points[farthest_point_idx] - m_points[edge.v1]));
        float dist_origin = glm::dot(normal, m_points[edge.v1]);

        // Use any point from the old hull to determine the correct orientation
        if (glm::dot(normal, m_points[face.v1] - m_points[edge.v1]) > 0) {
            normal = -normal;
            dist_origin = -dist_origin;
            m_hullFaces.emplace_back(edge.v1, farthest_point_idx, edge.v2, normal, dist_origin);
        }
        else {
            m_hullFaces.emplace_back(edge.v1, edge.v2, farthest_point_idx, normal, dist_origin);
        }
    }

    // 7. Reassign points to the new faces
    for (int point_idx : points_to_reassign) {
        if (point_idx == farthest_point_idx) continue;
        for (size_t i = new_faces_start_index; i < m_hullFaces.size(); ++i) {
            if (distanceToFace(m_hullFaces[i], m_points[point_idx]) > 1e-5f) {
                m_hullFaces[i].outsidePoints.push_back(point_idx);
                break;
            }
        }
    }

    // 8. Recursively expand any new faces that have outside points
    for (size_t i = new_faces_start_index; i < m_hullFaces.size(); ++i) {
        if (i < m_hullFaces.size() && !m_hullFaces[i].outsidePoints.empty()) {
            expandHull(m_hullFaces[i]);
        }
    }
}

float Quickhull3D::distanceToFace(const Face& face, const glm::vec3& point) const {
    return glm::dot(face.normal, point) - face.distanceToOrigin;
}