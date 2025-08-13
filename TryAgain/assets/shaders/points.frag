#version 330 core
out vec4 fragColor;

in vec3 ourColor;
uniform vec3 viewPos;
void main()
{
    vec2 coord  = gl_PointCoord-vec2(0.5);
    if (length(coord)>0.5){
    discard;
    }
    fragColor = vec4(ourColor, 1.0);
}
