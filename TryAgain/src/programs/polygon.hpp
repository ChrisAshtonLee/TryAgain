
#include <common/data.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <src/programs/program.h>
#include "../rendering/shader.h"
#include "../rendering/vertexmemory.hpp"
#include <vector>

//#include <algorithm>

#ifndef POLYGON_HPP
#define POLYGON_HPP

class Polygon : public Program {
    
   // std::vector<glm::vec3> colors;
    ArrayObject VAO;
    Shader shader;
  
   // std::vector <unsigned int> indices;
    glm::vec3 bc;
public:
    std::vector<Vertex> points;
	std::vector<glm::vec3> normals; // Normals for each point, default to zero if not provided
    std::vector <glm::vec3> vertices;
    //int noInstances =0;
    std::vector <unsigned int> indices;
	std::vector<glm::vec3> original_colors; // Store original colors for highlighting
    std::vector<int> indices_block_size;
    std::vector<bool> selected_indices;
    Polygon() :  shader(false, "polygon.vert", "polygon.frag") {
        noInstances = 0;
        VAO.generate(); // Generate VAO ID
        VAO.bind();     // Bind VAO
        // Initialize VBO within the VAO
        VAO["VBO"] = BufferObject(GL_ARRAY_BUFFER);
        VAO["VBO"].generate(); // Generate VBO ID
        VAO["VBO"].bind();     // Bind VBO
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(glm::vec3)));
        glEnableVertexAttribArray(1);
       
        VAO["EBO"] = BufferObject(GL_ELEMENT_ARRAY_BUFFER);
        VAO["EBO"].generate();
        VAO["EBO"].bind();
	}
    bool addInstance(std::vector<Vertex> p, std::vector<glm::vec3> norm_p = std::vector<glm::vec3>(0.0f))
    {
        if (noInstances >= 256) {
            std::cerr << "Maximum number of points reached." << std::endl;
            return false;
        }
        int index_offset = points.size();
		std::cout << "Adding instance with " << p.size() << " points." << std::endl;
        //points.insert(points.end(), p.begin(), p.end());
        for (const auto& point : p) {
            vertices.push_back(point.position);
            original_colors.push_back(point.color); // Store original color for highlighting
            points.push_back(point);
		}
        if (norm_p.size() > 1)
        {
            
             noInstances++;
			 std::cout << "normals are being used." << std::endl;
             normals= norm_p;
			 std::vector<unsigned int> temp_indices = tessellate(normals);
			 indices_block_size.push_back(temp_indices.size());
             for (const auto& index : temp_indices) {
                 indices.push_back(index + index_offset);
			 }
        }
        else {
            std::vector<unsigned int> temp_indices = tessellate(vertices);
            indices_block_size.push_back(temp_indices.size());
            for (const auto& index : temp_indices) {
                indices.push_back(index+index_offset);

            }
            for (const auto& index : indices) {
                std::cout << "index: " << index << " ";
            }
        }
        
        for (int i = 0; i< vertices.size(); ++i) {
            std::cout << "Point " << i << ": " << vertices[i].x << ", " << vertices[i].y << ", " << vertices[i].z << std::endl;
		}
        for (int i = 0; i < noInstances; ++i) {
            selected_indices.push_back(false);
        }
        return true;
    }
    int getInstanceCount() {
        return noInstances;
    }
    void updateInstances() {
      
        
        VAO.bind();
        VAO["VBO"].bind();
		VAO["VBO"].setData<Vertex>((GLuint)points.size(), &points[0], GL_STATIC_DRAW);
      
      
        
        VAO["EBO"].setData<GLuint>((GLuint)indices.size(), &indices[0], GL_STATIC_DRAW);
    }
    void deleteInstance(int idx) {
        if (idx < 0 || idx >= noInstances) {
            std::cerr << "Invalid index: " << idx << std::endl;
            return;
        }
        points.erase(points.begin() + idx);
        original_colors.erase(original_colors.begin() + idx); // Remove corresponding original color
		vertices.erase(vertices.begin() + idx);
        selected_indices.erase(selected_indices.begin()+idx);
        int startIdx = 0;
        for (int i = 0; i < idx; ++i) {
            startIdx += indices_block_size[i];
		}
		indices.erase(indices.begin() + startIdx, indices.begin()+startIdx+indices_block_size[idx]); 
        indices_block_size.erase(indices_block_size.begin() + idx);// Remove corresponding indices
        noInstances--;
        std::cout << "point deleted." << std::endl;
    }
    void clear() {
        std::cout << "clear pressed";
        points.clear();
        indices.clear();
        noInstances = 0;
		original_colors.clear(); // Clear original colors
        indices_block_size.clear();
        selected_indices.clear();

    }
    void render() {
        shader.activate();
        VAO.bind();
        //VAO.draw(GL_TRIANGLES, (GLuint)indices.size(), GL_UNSIGNED_INT, 0, noInstances);
        //VAO.drawDistinctElements(GL_TRIANGLES, indices_block_size, GL_UNSIGNED_INT, 0, noInstances);
        VAO.drawDistinctElements(GL_TRIANGLES, indices_block_size, GL_UNSIGNED_INT, noInstances);
    }

    void cleanup() {
        shader.cleanup();
        VAO.cleanup();
    }
    //bool HighlightPolygonIfHovered(int i, glm::vec3 mousePos)
    //{
    //    bool selected = false;
    //    int idx_start = 0;
    //    std::cout << "highlight polygon called. " << std::endl;
    //    
    //    for (int k = 0; k < i; ++k)
    //    {
    //        idx_start += indices_block_size[k];
    //    }
    //    int idx_stop = idx_start + indices_block_size[i];
    //    for (int p = idx_start; p < idx_stop - 3; p = p + 3)
    //    {
    //        if (isPointInTriangle(vertices[p], vertices[p + 1], vertices[p + 2], mousePos)){
    //            selected = true;
    //            std::cout << "Mouse in triangle: p1 = " << vertices[p].x << " " << vertices[p].y << " " << vertices[p].z << ", p2= " <<
    //                vertices[p + 1].x << " " << vertices[p + 1].y << " " << vertices[p + 1].z << ", p3= " <<
    //                vertices[p+2].x << " " << vertices[p+2].y << " " << vertices[p+2].z << std::endl;
    //        }
    //    }
    //    if (selected) {
    //        for (int p = idx_start; p < idx_stop; ++p) {
    //            points[p].color = glm::vec3(1.0f, 1.0f, 0.0f);
    //        }
    //    }
    //    else {
    //        for (int p = idx_start; p < idx_stop; ++p) {
    //            points[p].color = original_colors[p];
    //        }
    //    }
    //    updateInstances();
    //    return selected;
    //    //return offsets[i];
    //}

    bool HighlightPolygonIfHovered(int i, glm::vec3 mousePos,  glm::mat4 view,  glm::mat4 projection, float scr_width,float scr_height)
    {
        bool selected = false;
       // selected_indices[i] = false;
        int idx_start = 0;

        for (int k = 0; k < i; ++k)
        {
            idx_start += indices_block_size[k];
        }
        int idx_stop = idx_start + indices_block_size[i];
        glm::mat4 projView = projection * view;
        // Assuming a simple identity model matrix for this example
        glm::mat4 model = glm::mat4(1.0f);
        glm::vec4 viewport(0.0f, 0.0f, scr_width, scr_height);
        for (int p = idx_start; p < idx_stop; p += 3)
        {
            // Project the 3D triangle vertices to 2D screen space
            glm::vec3 screenPos1 = glm::project(vertices[indices[p]], model, projView, viewport);
            glm::vec3 screenPos2 = glm::project(vertices[indices[p + 1]], model, projView, viewport);
            glm::vec3 screenPos3 = glm::project(vertices[indices[p + 2]], model, projView, viewport);

            // The mouse position is already in 2D screen coordinates, but we need to flip the Y-axis
            // because screen coordinates typically have (0,0) at the top-left, while OpenGL's
            // window coordinates have (0,0) at the bottom-left.
            glm::vec3 mouseScreenPos = glm::vec3(mousePos.x, viewport[3]-mousePos.y, 0.0f);

            // Now perform the point-in-triangle test in 2D screen space
            if (isPointInTriangle(mouseScreenPos, screenPos1, screenPos2, screenPos3)) {
                selected = true;
                break; // Exit the loop once a triangle is found
            }
        }

        if (selected_indices[i]) {
            for (int p = idx_start; p < idx_stop; ++p) {
                points[indices[p]].color = glm::vec3(1.0f, 1.0f, 0.0f); // Highlight color
            }
        }
        else {
           
            for (int p = idx_start; p < idx_stop; ++p) {
                points[indices[p]].color = original_colors[indices[p]]; // Original color
            }
        }
        updateInstances();
        return selected;
    }

    void updateCameraMatrices(glm::mat4 projView, glm::vec3 camPos) {
        shader.activate();
        shader.setMat4("projView", projView);
        shader.set3Float("viewPos", camPos);

    }
    static bool compare(const std::pair<glm::vec3, float> &a, const std::pair<glm::vec3,float> &b) {
        return a.second<b.second;
    }
    std::vector<unsigned int> tessellate(std::vector<glm::vec3> verts) {
        for (int i = 0; i < verts.size(); ++i) {
            std::cout << "Point " << i << ": " << verts[i].x << ", " << verts[i].y << ", " << verts[i].z << std::endl;
		}
        std::vector<unsigned int> indices;
        if (verts.size() < 3) {
            // A polygon must have at least 3 vertices.
            return indices;
        }

        // Create a list of vertex indices to modify as we "clip" ears.
        std::vector<int> local_indices;
        local_indices.reserve(verts.size());
        for (size_t i = 0; i < verts.size(); ++i) {
            local_indices.push_back(i);
        }

        // Determine the polygon's winding order (clockwise or counter-clockwise)
        // by calculating the signed area of the entire polygon.
        float total_area = 0;
        for (size_t i = 0; i < verts.size(); ++i) {
            const glm::vec3& p1 = verts[local_indices[i]];
            const glm::vec3& p2 = verts[local_indices[(i + 1) % verts.size()]];
            total_area += (p2.x - p1.x) * (p2.y + p1.y);
        }
        const bool is_clockwise = total_area > 0;

        while (local_indices.size() > 3) {
            bool ear_clipped = false;
            for (size_t i = 0; i < local_indices.size(); ++i) {
                // Get indices for the potential ear triangle (prev, current, next)
                int prev_idx_ptr = (i == 0) ? local_indices.size() - 1 : i - 1;
                int curr_idx_ptr = i;
                int next_idx_ptr = (i + 1) % local_indices.size();

                int prev_v_idx = local_indices[prev_idx_ptr];
                int curr_v_idx = local_indices[curr_idx_ptr];
                int next_v_idx = local_indices[next_idx_ptr];

                const glm::vec3& prev_v = verts[prev_v_idx];
                const glm::vec3& curr_v = verts[curr_v_idx];
                const glm::vec3& next_v = verts[next_v_idx];

                // Check if the current vertex is a convex vertex (forms a valid ear tip).
                // This depends on the winding order.
                float ear_area = signedTriangleArea(prev_v, curr_v, next_v);
                if ((is_clockwise && ear_area < 0) || (!is_clockwise && ear_area > 0)) {
                    // This is a convex vertex, so it *could* be an ear.
                    bool is_ear = true;

                    // Check if any other vertex of the polygon lies inside this potential ear.
                    for (size_t j = 0; j < points.size(); ++j) {
                        if (j == prev_v_idx || j == curr_v_idx || j == next_v_idx) {
                            continue; // Don't check the ear's own vertices.
                        }
                        if (isPointInTriangle(verts[j], prev_v, curr_v, next_v)) {
                            is_ear = false;
                            break; // Not an ear, try the next vertex.
                        }
                    }

                    if (is_ear) {
                        // It's a valid ear! Add its indices to our output list.
                        indices.push_back(prev_v_idx);
                        indices.push_back(curr_v_idx);
                        indices.push_back(next_v_idx);

                        // "Clip" the ear by removing its tip from our local index list.
                        local_indices.erase(local_indices.begin() + curr_idx_ptr);
                        ear_clipped = true;
                        break; // Restart the loop with the smaller polygon.
                    }
                }
            }
            if (!ear_clipped) {
                // This can happen with complex, non-simple polygons or if something went wrong.
                // Returning an empty list indicates failure.
				std::cout << "Failed to triangulate polygon, no valid ears found." << std::endl;
                return {};
            }
        }

        // Add the final remaining triangle.
        indices.push_back(local_indices[0]);
        indices.push_back(local_indices[1]);
        indices.push_back(local_indices[2]);

        return indices;
    }
    float signedTriangleArea(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c) {
		
        return (a.x - c.x) * (b.y - c.y) - (a.y - c.y) * (b.x - c.x);
    }

    // Helper function to check if a point 'p' is inside the triangle defined by a, b, c.
    // This is used to ensure no other vertices are inside a potential "ear".
    bool isPointInTriangle(const glm::vec3& p, const glm::vec3& a, const glm::vec3& b, const glm::vec3& c) {
        float d1 = signedTriangleArea(p, a, b);
        float d2 = signedTriangleArea(p, b, c);
        float d3 = signedTriangleArea(p, c, a);

        bool has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
        bool has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

        return !(has_neg && has_pos);
    }
};
#endif