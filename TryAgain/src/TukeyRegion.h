#ifndef TUKEYREGION_H
#define TUKEYREGION_H

#include <vector>
#include <utility>

using namespace std;

class TukeyRegion {
private:
    vector<vector<float>> lines;
    vector<vector<float>> klevels;
    vector<vector<float>> points;
    vector<vector<float>> upper_klevels;
    vector<vector<float>> lower_klevels;
    vector<float> tempvec;
    vector<pair<float, float>> contourVertices;

    int num_lines;
    float a1, b1, a2, b2;

    vector<float> dual_intersect(float a1, float b1, float a2, float b2);
    void vert_intersect(vector<float> point);
    vector<float> makeUnique(vector<float> list);
    vector<pair<float, float>> makeUniquePairs(vector<pair<float, float>> arr);

public:
    TukeyRegion(const vector<vector<float>>& lines, int num_lines);

    vector<vector<float>> calc();
    vector<pair<float, float>> k_contour(int k);
};

#endif // TUKEYREGION_HPP