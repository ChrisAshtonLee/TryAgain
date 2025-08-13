#pragma once

#include <vector>
#include <random>
#include <numeric>
#include <algorithm>
#include <iostream>
#include <glm/glm.hpp>
#include <src/geometry_primitive/points.hpp>
#include<src/TukeyContour.h>
#include <common/data.h>
#include <cstdlib>
#include <ctime>
#include<scripts/sym_projection.h>

class ResilientConsensus {
public:
    int n;
    int f;
    std::vector<int> normal_agents;
    std::vector<int> attackers;
    std::vector<glm::vec2> X;
    ProjectionResult Xp;
    std::vector<std::vector<int>> Nh;
    std::vector<std::vector<glm::vec2> > X_history;
   
    int attack_mode = 1;
    bool is_running = false;
    int current_step = 0;
    int max_steps = 500;
    bool suppress = true;
    ResilientConsensus(std::vector<int> norm_agents, std::vector<int> adv_agents, std::shared_ptr<Points> pointsPtr) : normal_agents(norm_agents), attackers(adv_agents), m_points(pointsPtr) {
        // Initialize agent positions
        n = normal_agents.size();
        f = attackers.size();
        X.resize(n+f);
        for (int i : normal_agents)
        {
            X[i] = glm::vec2(m_points->points[i].position.z, m_points->points[i].position.y);
            std::cout << "X[" << i << "]: " << X[i].x << ", " << X[i].y << std::endl;
        }
        for (int i : attackers)
        {
            X[i] = glm::vec2(m_points->points[i].position.z, m_points->points[i].position.y);
            std::cout << "X[" << i << "]: " << X[i].x << ", " << X[i].y << std::endl;
        }

        // Assign attackers
       /* attackers.resize(f);
        for (int i = 0; i < f; ++i) {
            attackers[i] = n - 1 - i;
        }*/
        std::random_device rd;
        std::default_random_engine rng(rd());
        // Initialize neighborhoods
        Nh.resize(n+f);
    
        for (int i : normal_agents) {
          
            std::vector<int> normal_agent_ind(normal_agents);
         
            std::shuffle(normal_agent_ind.begin(), normal_agent_ind.end(), rng);

            Nh[normal_agents[i]].assign(normal_agent_ind.begin(), normal_agent_ind.begin() + std::min((int)normal_agent_ind.size(), 6));
            
            for (int attacker : attackers) {
                Nh[normal_agents[i]].push_back(attacker);
            }
        }
       
    }

    void sim_step() {
        if (!is_running || current_step >= max_steps) {
            is_running = false; // Stop when done
            return;
        }

        float dt = 0.01f;
        std::vector<glm::vec2> targets(n + f); // Resize to handle all agents

        // --- This is the body of your old for-loop ---
        for (int i : normal_agents) { // Loop over ALL agents (n+f)
            std::vector<glm::vec2> Xn;
            for (int j : Nh[normal_agents[i]])
            {
                Xn.push_back(X[j]);
            }
            Xp = TV_Projection(Xn);
           // targets[i] = get_centerpoint(i);
            targets[i] = get_contour(i, Xp.adjusted_points);
          
        }
        for (int a_idx : attackers) {
            if (attack_mode == 1)      targets[a_idx] = X[a_idx];
            else if (attack_mode == 2) targets[a_idx] = get_attacker_target(a_idx);
            else                       targets[a_idx] = X[a_idx];
            if (!suppress) std::cout << "Attacker " << a_idx << " has target" << targets[a_idx].x << " " << targets[a_idx].y << std::endl;
        }

        // Update positions
        for (int i : normal_agents) { // Loop over ALL agents
         
         
              X[i] += dt * (targets[i] - X[i]);  
              m_points->points[i].position.z = X[i].x;
              m_points->points[i].position.y = X[i].y;
             
              
        }
        for (int i : attackers) { // Loop over ALL agents

            X[i] += dt * (targets[i] - X[i]);
            m_points->points[i].position.z = X[i].x;
            m_points->points[i].position.y = X[i].y;
        }
        m_points->load();
        
        X_history.push_back(X);
      
        current_step++;
    }
    void start_sim(int max_step) {
        current_step = 0;
        max_steps = max_step;
        is_running = true;
        X_history.clear(); // Clear history from previous runs
    }

private:
    std::shared_ptr<Points> m_points{};
    glm::vec2 get_centerpoint(int agent_index) {
        std::vector<Vertex> neighbor_positions;
        for (int neighbor_idx : Nh[agent_index]) {
           if (!suppress) std::cout << " Neighbor index: " << Nh[agent_index][neighbor_idx] << std::endl;
            neighbor_positions.push_back({ glm::vec3(X[neighbor_idx].x, X[neighbor_idx].y, 0.0f), glm::vec3(0.0f) });
           if (!suppress) std::cout << "Position: " << X[neighbor_idx].x << X[neighbor_idx].y << std::endl;
        }

        TukeyContour TC(neighbor_positions, 1, false);
        if (!TC.median_contour.empty()) {
            glm::vec3 center(0.0f);
            for (const auto& v : TC.median_contour) {
                center += v.position;
            }
            center /= (float)TC.median_contour.size();
           if (!suppress) std::cout << "Agent " << agent_index << " has target " << center.x << " " << center.y << std::endl;
            return glm::vec2(center.x, center.y);
        }
       if (!suppress) std::cout << "Agent " << agent_index << " did not move." << std::endl;
        return X[agent_index]; // Fallback
    }

    glm::vec2 get_contour(int agent_index, std::vector<glm::vec2> Xa) {
        std::vector<Vertex> neighbor_positions;
        for (glm::vec2 pos : Xa) {
           // if (!suppress) std::cout << " Neighbor index: " << Nh[agent_index][neighbor_idx] << std::endl;
            neighbor_positions.push_back({ glm::vec3(pos.x, pos.y, 0.0f), glm::vec3(0.0f) });
            //if (!suppress) std::cout << "Position: " << X[neighbor_idx].x << X[neighbor_idx].y << std::endl;
        }

        TukeyContour TC(neighbor_positions, 1, false);
        if (!TC.median_contour.empty()) {
            glm::vec3 center(0.0f);
            for (const auto& v : TC.median_contour) {
                center += v.position;
            }
            center /= (float)TC.median_contour.size();
            if (!suppress) std::cout << "Agent " << agent_index << " has target " << center.x << " " << center.y << std::endl;
            return glm::vec2(center.x, center.y);
        }
        if (!suppress) std::cout << "Agent " << agent_index << " did not move." << std::endl;
        return X[agent_index]; // Fallback
    }

    glm::vec2 get_attacker_target(int attacker_idx) {
        bool first_found = false;
        glm::vec2 target = X[attacker_idx];

        for (size_t i = 0; i < Nh.size(); ++i) {
            if (std::find(attackers.begin(), attackers.end(), i) == attackers.end()) { // is honest agent
                for (int neighbor_in_hood : Nh[i]) {
                    if (neighbor_in_hood == attacker_idx) {
                        std::vector<glm::vec2> temp_hood;
                        for (int honest_neighbor : Nh[i]) {
                            if (honest_neighbor != attacker_idx) {
                                temp_hood.push_back(X[honest_neighbor]);
                            }
                        }

                        // Simplified target logic: move towards the mean of the honest neighbors in that hood
                        glm::vec2 mean(0.0f, 0.0f);
                        if (!temp_hood.empty()) {
                            for (const auto& pos : temp_hood) {
                                mean += pos;
                            }
                            mean /= (float)temp_hood.size();
                        }
                        // A simple attack could be to move to the opposite side of the mean
                        return X[attacker_idx] + (X[attacker_idx] - mean);
                    }
                }
            }
        }
        return target;
    }
};