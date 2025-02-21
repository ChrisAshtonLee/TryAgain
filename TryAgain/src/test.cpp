#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Function to convert world coordinates to window coordinates
glm::vec2 worldToScreen(const glm::vec3& worldPos, const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection, int screenWidth, int screenHeight) {
    // Model-View-Projection (MVP) matrix
    glm::mat4 MVP = projection * view * model;

    // Transform world coordinates to Normalized Device Coordinates (NDC)
    glm::vec4 clipSpacePos = MVP * glm::vec4(worldPos, 1.0f);

    // Perspective division to transform to NDC
    glm::vec3 ndcPos = glm::vec3(clipSpacePos) / clipSpacePos.w;

    // Convert NDC to screen coordinates
    glm::vec2 screenPos;
    screenPos.x = (ndcPos.x + 1.0f) * 0.5f * screenWidth;
    screenPos.y = (1.0f - ndcPos.y) * 0.5f * screenHeight;  // Note the inverted y-axis

    return screenPos;
}

int main() {
    // Define the world position of the vertex
    glm::vec3 worldPos = glm::vec3(1.0f, 2.0f, 0.0f);

    // Initialize the Model, View, and Projection matrices
    glm::mat4 model = glm::mat4(1.0f);  // Identity matrix for the model
    glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);

    // Define screen dimensions
    int screenWidth = 800;
    int screenHeight = 600;

    // Convert world position to screen coordinates
    glm::vec2 screenPos = worldToScreen(worldPos, model, view, projection, screenWidth, screenHeight);

    // Print the screen coordinates
    printf("Screen Position: x = %f, y = %f\n", screenPos.x, screenPos.y);

    return 0;
}
