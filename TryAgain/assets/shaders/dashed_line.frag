#version 130

in vec3 vColor;
in float vLineDistance; // Interpolated distance from the start

out vec4 FragColor;

// You can adjust these values for different dash styles
uniform float u_dashSize = 0.01;
uniform float u_gapSize = 0.005;

void main() {
    float patternLength = u_dashSize + u_gapSize;
    
    // Use mod to see where we are in the repeating dash-gap pattern
    if (mod(vLineDistance, patternLength) > u_dashSize) {
        discard; // This fragment is in a gap, so don't draw it
    }
    
    FragColor = vec4(vColor, 1.0);
}