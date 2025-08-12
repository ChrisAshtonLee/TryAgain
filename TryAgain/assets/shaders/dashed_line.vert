#version 130

in vec3 aPos;
in vec3 aColor;
in float aLineDistance; // Distance from the start of the line segment

out vec3 vColor;
out float vLineDistance; // Pass distance to fragment shader

uniform mat4 u_mvp;

void main() {
    gl_Position = u_mvp * vec4(aPos, 1.0);
    vColor = aColor;
    vLineDistance = aLineDistance;
}