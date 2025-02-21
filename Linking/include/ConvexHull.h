#ifndef CONVEXHULL_H
#define CONVEXHULL_H

#include <vector>
#include <utility>
using namespace std;
extern pair<float, float> mid;
	
int quad(pair<float, float> p);
int orientation(pair<float, float> a, pair<float, float> b, pair<float, float> c);
bool compare(pair<float, float> p1, pair<float, float> q1);
vector<pair<float, float>> merger(vector<pair<float, float>> a, vector<pair<float, float>> b);
vector<pair<float, float>> bruteHull(vector<pair<float, float>> a);
vector<pair<float, float>> divide(vector<pair<float, float>> a);
vector<pair<float, float>> ConvHull(vector<pair<float, float>> a);

#endif
