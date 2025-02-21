#include "ConvexHull.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <set>

#include <utility>


using namespace std;
pair<float, float> mid;

    int n;
    // determines the quadrant of a point
    // (used in compare())
    int quad(pair<float, float> p)
    {
        if (p.first >= 0 && p.second >= 0)
            return 1;
        if (p.first <= 0 && p.second >= 0)
            return 2;
        if (p.first <= 0 && p.second <= 0)
            return 3;
        return 4;
    }

    // Checks whether the line is crossing the polygon
    int orientation(pair<float, float> a, pair<float, float> b, pair<float, float> c)
    {
        float res = (b.second - a.second) * (c.first - b.first) - (c.second - b.second) * (b.first - a.first);

        if (res == 0)
            return 0;
        if (res > 0)
            return 1;
        return -1;
    }

    // compare function for sorting
    bool compare(pair<float, float> p1, pair<float, float> q1)
    {
        pair<float, float> p = make_pair(p1.first - mid.first, p1.second - mid.second);
        pair<float, float> q = make_pair(q1.first - mid.first, q1.second - mid.second);

        int one = quad(p);
        int two = quad(q);

        if (one != two)
            return (one < two);
        return (p.second * q.first < q.second * p.first);
    }

    // Finds upper tangent of two polygons 'a' and 'b'
    // represented as two vectors.
    vector<pair<float, float>> merger(vector<pair<float, float>> a, vector<pair<float, float>> b)
    {
        // n1 -> number of points in polygon a
        // n2 -> number of points in polygon b
        int n1 = a.size(), n2 = b.size();

        int ia = 0, ib = 0;
        for (int i = 1; i < n1; i++)
            if (a[i].first > a[ia].first)
                ia = i;

        // ib -> leftmost point of b
        for (int i = 1; i < n2; i++)
            if (b[i].first < b[ib].first)
                ib = i;

        // finding the upper tangent
        int inda = ia, indb = ib;
        bool done = 0;
        while (!done)
        {
            done = 1;
            while (orientation(b[indb], a[inda], a[(inda + 1) % n1]) >= 0)
                inda = (inda + 1) % n1;

            while (orientation(a[inda], b[indb], b[(n2 + indb - 1) % n2]) <= 0)
            {
                indb = (n2 + indb - 1) % n2;
                done = 0;
            }
        }

        int uppera = inda, upperb = indb;
        inda = ia, indb = ib;
        done = 0;
        int g = 0;
        while (!done) // finding the lower tangent
        {
            done = 1;
            while (orientation(a[inda], b[indb], b[(indb + 1) % n2]) >= 0)
                indb = (indb + 1) % n2;

            while (orientation(b[indb], a[inda], a[(n1 + inda - 1) % n1]) <= 0)
            {
                inda = (n1 + inda - 1) % n1;
                done = 0;
            }
        }

        int lowera = inda, lowerb = indb;
        vector<pair<float, float>> ret;

        // ret contains the convex hull after merging the two convex hulls
        // with the points sorted in anti-clockwise order
        int ind = uppera;
        ret.push_back(a[uppera]);
        while (ind != lowera)
        {
            ind = (ind + 1) % n1;
            ret.push_back(a[ind]);
        }

        ind = lowerb;
        ret.push_back(b[lowerb]);
        while (ind != upperb)
        {
            ind = (ind + 1) % n2;
            ret.push_back(b[ind]);
        }
        return ret;
    }

    // Brute force algorithm to find convex hull for a set
    // of less than 6 points
    vector<pair<float, float>> bruteHull(vector<pair<float, float>> a)
    {
        // Take any pair of points from the set and check
        // whether it is the edge of the convex hull or not.
        // if all the remaining points are on the same side
        // of the line then the line is the edge of convex
        // hull otherwise not
        set<pair<float, float>> s;

        for (int i = 0; i < a.size(); i++)
        {
            for (int j = i + 1; j < a.size(); j++)
            {
                float x1 = a[i].first, x2 = a[j].first;
                float y1 = a[i].second, y2 = a[j].second;

                float a1 = y1 - y2;
                float b1 = x2 - x1;
                float c1 = x1 * y2 - y1 * x2;
                int pos = 0, neg = 0;
                for (int k = 0; k < a.size(); k++)
                {
                    if (a1 * a[k].first + b1 * a[k].second + c1 <= 0)
                        neg++;
                    if (a1 * a[k].first + b1 * a[k].second + c1 >= 0)
                        pos++;
                }
                if (pos == a.size() || neg == a.size())
                {
                    s.insert(a[i]);
                    s.insert(a[j]);
                }
            }
        }

        vector<pair<float, float>> ret;
        for (auto e : s)
            ret.push_back(e);

        // Sorting the points in the anti-clockwise order
        mid = { 0.0, 0.0 };
        int n = ret.size();
        for (int i = 0; i < n; i++)
        {
            mid.first += ret[i].first;
            mid.second += ret[i].second;
            ret[i].first *= n;
            ret[i].second *= n;
        }
        sort(ret.begin(), ret.end(), compare);
        for (int i = 0; i < n; i++)
            ret[i] = make_pair(ret[i].first / n, ret[i].second / n);

        return ret;
    }

    // Returns the convex hull for the given set of points
    vector<pair<float, float>> divide(vector<pair<float, float>> a)
    {
        // If the number of points is less than 6 then the
        // function uses the brute algorithm to find the
        // convex hull
        if (a.size() <= 5)
            return bruteHull(a);

        // left contains the left half points
        // right contains the right half points
        vector<pair<float, float>> left, right;
        for (int i = 0; i < a.size() / 2; i++)
            left.push_back(a[i]);
        for (int i = a.size() / 2; i < a.size(); i++)
            right.push_back(a[i]);

        // convex hull for the left and right sets
        vector<pair<float, float>> left_hull = divide(left);
        vector<pair<float, float>> right_hull = divide(right);

        // merging the convex hulls
        return merger(left_hull, right_hull);
    }
    vector<pair<float, float>> ConvHull(vector<pair<float, float>> a)
    {

        vector<pair<float, float>> ans;

        int n = a.size();
        if (n > 0) {
            sort(a.begin(), a.end());
            ans = divide(a);
        }
        // sorting the set of points according
        // to the x-coordinate

        else {
            return vector<pair<float, float>>();
        }


        return ans;
    }


