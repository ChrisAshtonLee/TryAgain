#include <common/_math.h>


Point operator+(const Point& a, const Point& b) { return { a.x + b.x, a.y + b.y }; }
Point operator-(const Point& a, const Point& b) { return { a.x - b.x, a.y - b.y }; }
Point operator/(const Point& a, double scalar) { return { a.x / scalar, a.y / scalar }; }


double dot(const Point& a, const Point& b) { return a.x * b.x + a.y * b.y; }
double norm(const Point& a) { return std::sqrt(a.x * a.x + a.y * a.y); }
double cross_product(Point p1, Point p2, Point p3) {
    return (p2.x - p1.x) * (p3.y - p1.y) - (p2.y - p1.y) * (p3.x - p1.x);
}
std::optional<Point> _find_intersection(const Point& p1, const Point& p2, const Point& p3, const Point& p4) {
    
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

     Point intersection;
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