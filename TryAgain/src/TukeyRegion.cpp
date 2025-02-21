//#ifndef TUKEYREGION_HPP
//#define TUKEYREGION_HPP
#include "TukeyRegion.h"
#include "ConvexHull.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include <utility>
#include <set>
#include <algorithm>


TukeyRegion::TukeyRegion(const vector<vector<float>>& lines, int num_lines) : lines(lines), num_lines(lines.size()) {}

    vector<vector<float >> TukeyRegion::calc() {

        for (int i = 0; i <= num_lines; ++i) {
            klevels.push_back(vector<float>());
            upper_klevels.push_back(vector<float>());
            lower_klevels.push_back(vector<float>());
        }
        for (int n = 0; n < num_lines; ++n) {
            if (n < num_lines - 1) {
                for (int m = n + 1; m < num_lines; ++m) {
                    a1 = lines[n][0];  b1 = lines[n][1];
                    a2 = lines[m][0]; b2 = lines[m][1];
                    if (a1 - a2 != 0) {
                        vector<float> point = dual_intersect(-a1, b1, -a2, b2);
                        vert_intersect(point);
                    }
                }
            }
        }
        std::cout << "upper klevels: " << std::endl;
        for (int j = 0; j < upper_klevels.size(); ++j) {
            std::cout << "upper " << j << " levels: ";
            for (auto e : upper_klevels[j])
                std::cout << e << ", ";
            std::cout << endl;
        }
        std::cout << endl;
        std::cout << "lower klevels: " << std::endl;
        for (int j = 0; j < lower_klevels.size(); ++j) {
            std::cout << "lower " << j << " levels: ";
            for (auto e : lower_klevels[j])
                std::cout << e << ", ";
            std::cout << endl;
        }
        std::cout << endl;

        

        return klevels;
    }

    vector<float> TukeyRegion::dual_intersect(float a1, float b1, float a2, float b2) {
        
        float x = (b2 - b1) / (a1 - a2);
        float y = a1 * x + b1;
        vector<float> answer = { x,y };
       // std::cout << "for "<<a1<<","<< b1<< " "<< a2 << "," << b2 << " we have: " << "x: " << x << " y : " << y << std::endl;
        return answer;
    }
    void TukeyRegion::vert_intersect(vector <float> point) {

        int upper = 0;
        int lower = 0;
        int k;
        for (int i = 0; i < num_lines; ++i) {
            float a = -lines[i][0];
            float b = lines[i][1];
            if (a * point[0] + b > point[1]) {
                upper++;
            }
            if (a*point[0]+b <point[1])
            {
                lower++;
            }
            
        }
        if (lower < upper) {
            k = lower;
           
            for (int i = 0; i <= k; ++i) {
                tempvec = lower_klevels[k];
                tempvec.push_back(point[0]);
                tempvec.push_back(point[1]);
                lower_klevels[k] = tempvec;
                tempvec.clear();

            }
            
        }
        else {
            if (upper < lower) {
                k = upper;
               
             
                for (int i = 0; i <= k; ++i) {
                    tempvec = upper_klevels[k];
                    tempvec.push_back(point[0]);
                    tempvec.push_back(point[1]);
                    upper_klevels[k] = tempvec;
                    tempvec.clear();

                    }
                
            }
            else {
                k = 0;
            }
        }
        if (upper == lower && upper!=0) {
            k = upper;
        }
        for (int i = 0; i <= k; ++i) {
            tempvec = klevels[i];
            
            tempvec.push_back(point[0]);
            tempvec.push_back(point[1]);
            klevels[i] = tempvec;
            tempvec.clear();
           
        }
        
        for (int i = 0; i < klevels.size(); ++i) {
            if (klevels[i].size() > 1) {
                klevels[i] = makeUnique(klevels[i]);
            }
        }
        
        //tempvec.clear();

    }


    vector<pair<float, float>> TukeyRegion::k_contour(int k) {

        if (klevels[k].size() < 1) {
            return vector<pair<float,float>>();
        }
        vector<pair<float, float>> primalPoints;
      
        for (int i = 0; i < klevels[k].size() - 2; i += 2) {
          
            a1 = klevels[k][i]; b1 = klevels[k][i + 1];
            
           
            for (int j = i + 2; j < klevels[k].size(); j += 2) {
                a2 = klevels[k][j]; b2 = klevels[k][j + 1];
                
                if (a1 - a2 != 0) {
                    vector<float> point = dual_intersect(a1, b1,a2, b2);
  
                    primalPoints.push_back(make_pair(point[0], point[1]));                                       
                }
            }
        }
        primalPoints = makeUniquePairs(primalPoints);
        for (auto e : primalPoints) {
            std::cout << "Primal Points:" << e.first << " " << e.second  << std::endl;
        }

        vector<pair<float, float>> khull;
        bool upper_intersection;
        bool lower_intersection;
        for (int i = 0; i < primalPoints.size(); ++i) {
            upper_intersection = true;
            lower_intersection = true;
            std::cout << "primal point " << i << ": " << primalPoints[i].first << " " << primalPoints[i].second << std::endl;
            for (int j = 0; j < upper_klevels[k].size(); j = j + 2) {
                if (primalPoints[i].second > float(upper_klevels[k][j] * primalPoints[i].first + upper_klevels[k][j + 1])) {
                    upper_intersection = false;
                    std::cout << "primal point: " << primalPoints[i].first << ", " << primalPoints[i].second << " is below " << upper_klevels[k][j] << ", " << upper_klevels[k][j + 1] << std::endl;
                }
                else {
                    float q = upper_klevels[k][j] * primalPoints[i].first + upper_klevels[k][j + 1];
                    std::cout << "GOOD " << primalPoints[i].second << " less than " << q << endl;
                
                }
                }
            
            for (int j = 0; j < lower_klevels[k].size(); j = j + 2) {
                if (primalPoints[i].second < float(lower_klevels[k][j] * primalPoints[i].first + lower_klevels[k][j + 1])) {
                    lower_intersection = false;
                    std::cout << "primal point: " << primalPoints[i].first << ", " << primalPoints[i].second << " is above " << lower_klevels[k][j] << ", " << lower_klevels[k][j + 1] << std::endl;
                }
                else {
                    float q = lower_klevels[k][j] * primalPoints[i].first + lower_klevels[k][j + 1];
                    std::cout << "GOOD " << primalPoints[i].second << " is greater than " << q << endl;

                }
            }
            std::cout << "upper_intersection: " << upper_intersection << std::endl;
            std::cout << "lower_intersection: " << lower_intersection << std::endl;
            if (upper_intersection== true && lower_intersection==true) {
               // std::cout << "successfully added: " << primalPoints[i].first << ", " << primalPoints[i].second << std::endl;
                khull.push_back(primalPoints[i]);
            }
        }
        
        khull = makeUniquePairs(khull);
        std::cout << "khull: " << std::endl;
        for (auto h : khull) {
            std::cout << h.first << " " << h.second << std::endl;
        }
        if (khull.size() == 1) {
            return khull;
        }
        else {
            return ConvHull(khull);
        }
    }

    vector<float> TukeyRegion::makeUnique(vector<float> arr) {
        bool inlist = false;
        vector<int> uniqueIdxs;
        vector<float> uniqueList;
        for (int i = 0; i < arr.size() - 1; i +=2) {
            inlist = false;
            //cout << " do this once:" << i<<" " << arr.size() << endl;
            if (uniqueIdxs.size() > 0) {
                for (int j = 0; j < uniqueIdxs.size(); ++j) {
                    if (arr[i] == arr[uniqueIdxs[j]] && arr[i + 1] == arr[uniqueIdxs[j] + 1]) {
                        inlist = true;
                    }
                }
                if (!inlist) {
                    uniqueIdxs.push_back(i);
                }
            }
            else {
                uniqueIdxs.push_back(i);
            }
        }
        for (int i = 0; i < uniqueIdxs.size(); ++i) {
            uniqueList.push_back(arr[uniqueIdxs[i]]);
            uniqueList.push_back(arr[uniqueIdxs[i] + 1]);
        }
        return uniqueList ;
    }
    vector<pair<float,float>> TukeyRegion::makeUniquePairs(vector<pair<float,float>> arr) {
        bool inlist = false;
        vector<int> uniqueIdxs;
        vector<pair<float,float>> uniqueList;
        for (int i = 0; i < arr.size(); ++i) {
            inlist = false;
            //cout << " do this once:" << i<<" " << arr.size() << endl;
            if (uniqueIdxs.size() > 0) {
                for (int j = 0; j < uniqueIdxs.size(); ++j) {
                    //if (arr[i] == arr[uniqueIdxs[j]]) {
                    if (std::abs(arr[i].first- arr[uniqueIdxs[j]].first)<.0001 && std::abs(arr[i].second - arr[uniqueIdxs[j]].second)<.0001){
                        inlist = true;
                    }
                }
                if (!inlist) {
                    uniqueIdxs.push_back(i);
                }
            }
            else {
                uniqueIdxs.push_back(i);
            }
        }
        for (int i = 0; i < uniqueIdxs.size(); ++i) {
            uniqueList.push_back(arr[uniqueIdxs[i]]);
            
        }
        return uniqueList;
    }


//#endif
