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
        if (!m_hullFaces[i].outsidePoints.empty()) {
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
    // 1. Find the 6 extreme points (min/max for x, y, z)
    std::vector<int> extreme_indices(6);
    std::vector<float> extreme_values = {
        points[0].x, points[0].x, points[0].y, points[0].y, points[0].z, points[0].z
    };

    for (size_t i = 1; i < points.size(); ++i) {
        if (points[i].x < extreme_values[0]) { extreme_values[0] = points[i].x; extreme_indices[0] = i; }
        if (points[i].x > extreme_values[1]) { extreme_values[1] = points[i].x; extreme_indices[1] = i; }
        if (points[i].y < extreme_values[2]) { extreme_values[2] = points[i].y; extreme_indices[2] = i; }
        if (points[i].y > extreme_values[3]) { extreme_values[3] = points[i].y; extreme_indices[3] = i; }
        if (points[i].z < extreme_values[4]) { extreme_values[4] = points[i].z; extreme_indices[4] = i; }
        if (points[i].z > extreme_values[5]) { extreme_values[5] = points[i].z; extreme_indices[5] = i; }
    }

    // 2. Find the four unique points for the initial tetrahedron
    std::set<int> initial_indices;
    for (int idx : extreme_indices) initial_indices.insert(idx);

    if (initial_indices.size() < 4) return false; // Degenerate case

    std::vector<int> initial_points(initial_indices.begin(), initial_indices.end());
    int v1 = initial_points[0];
    int v2 = initial_points[1];
    int v3 = initial_points[2];
    int v4 = initial_points[3]; // A bit simplistic, but often works. A robust way would find the largest tetrahedron.

    // 3. Create the initial four faces, ensuring they face outwards
    glm::vec3 centroid = (m_points[v1] + m_points[v2] + m_points[v3] + m_points[v4]) / 4.0f;

    auto create_face = [&](int a, int b, int c) {
        glm::vec3 normal = glm::normalize(glm::cross(m_points[b] - m_points[a], m_points[c] - m_points[a]));
        float dist = glm::dot(normal, m_points[a]);

        // Ensure the normal points outwards from the centroid
        if (glm::dot(normal, m_points[a] - centroid) < 0) {
            normal = -normal;
            dist = -dist;
            std::swap(b, c); // Maintain CCW order
        }
        m_hullFaces.emplace_back(a, b, c, normal, dist);
        };

    create_face(v1, v2, v3);
    create_face(v1, v3, v4);
    create_face(v1, v4, v2);
    create_face(v2, v4, v3);

    // 4. Assign all other points to the outside set of the first face they are outside of
    std::set<int> hull_points = { v1, v2, v3, v4 };
    for (size_t i = 0; i < points.size(); ++i) {
        if (hull_points.count(i)) continue;

        for (auto& face : m_hullFaces) {
            if (distanceToFace(face, points[i]) > 1e-5f) {
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
    if (farthest_point_idx == -1) return; // Should not happen if outsidePoints is not empty

    // 2. Identify all visible faces from the farthest point
    std::vector<Face*> visible_faces;
    std::map<std::pair<int, int>, int> edge_counts; // Edge -> number of visible faces sharing it

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

    // 3. Find the horizon edges (edges shared by only one visible face)
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
        float dist = glm::dot(normal, m_points[edge.v1]);

        // Ensure new faces point outwards
        if (distanceToFace({ edge.v1, edge.v2, farthest_point_idx, normal, dist }, m_points[face.v1]) > 0) {
            normal = -normal;
            dist = -dist;
            m_hullFaces.emplace_back(edge.v1, farthest_point_idx, edge.v2, normal, dist);
        }
        else {
            m_hullFaces.emplace_back(edge.v1, edge.v2, farthest_point_idx, normal, dist);
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
        if (!m_hullFaces[i].outsidePoints.empty()) {
            expandHull(m_hullFaces[i]);
        }
    }
}

float Quickhull3D::distanceToFace(const Face& face, const glm::vec3& point) const {
    return glm::dot(face.normal, point) - face.distanceToOrigin;
}