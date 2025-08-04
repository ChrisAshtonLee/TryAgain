#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

void main()
{
    // Semi-transparent blue color
    FragColor = vec4(0.2, 0.4, 0.8, 0.3);
} 