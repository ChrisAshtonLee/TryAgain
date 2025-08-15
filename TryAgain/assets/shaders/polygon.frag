#version 330 core
out vec4 fragColor;

in vec3 ourColor;
uniform vec3 viewPos;
uniform float alpha;
void main()
{
    fragColor = vec4(ourColor, alpha);
}
