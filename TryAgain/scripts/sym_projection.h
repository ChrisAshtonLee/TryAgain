#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include<common/data.h>
#include<common/_math.h>

ProjectionResult TV_Projection(const std::vector<Point>& points);
bool is_dividing_line(const std::vector<Point>& points, const std::pair<int, int>& line_indices);
