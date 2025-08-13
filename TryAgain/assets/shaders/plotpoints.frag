#version 330 core

in vec3 ourColor;
out vec4 fragColor;
uniform vec3 viewPos;

void main()
{
    // 1. Calculate distance from the center of the point.
    // gl_PointCoord ranges from (0,0) to (1,1). We remap it to (-0.5, -0.5) to (0.5, 0.5)
    // to make the center (0,0), which simplifies the distance calculation.
    vec2 coord = gl_PointCoord - vec2(0.5);
    float distance = length(coord);
    float delta = fwidth(distance);
    // 2. Create a smooth, anti-aliased edge for the circle.
    // We use smoothstep to create a soft transition between the inside and outside of the circle.
    // The alpha will be 1.0 inside a radius of 0.45, and fade to 0.0 by radius 0.5.
    float alpha = 1.0 - smoothstep(0.5-delta, 0.5+delta, distance);

    // 3. Create the darker border.
    // This creates a gradient near the edge of the circle.
    // The `border` will be 0.0 for most of the point and quickly become 1.0 near the edge.
    float border_edge = 0.47;
    float border = smoothstep(border_edge - delta, border_edge + delta, distance);
    
    // Mix the original color with a darker version based on the border value.
    vec3 finalColor = mix(ourColor, ourColor * 0.5, border);

    // 4. Discard fully transparent pixels and set final color.
    // This is an important optimization that can improve performance.
  
    if (alpha < 0.01) {
        discard;
     }
    fragColor = vec4(finalColor, alpha);
}