#version 330 core
layout (location = 0) in vec2 aPos;

uniform vec2 screenSize;
uniform vec2 startPos;
uniform vec2 endPos;

out vec2 TexCoord;

void main()
{
    // Calculate the selection box rectangle
    vec2 minPos = min(startPos, endPos);
    vec2 maxPos = max(startPos, endPos);
    
    // Convert to normalized device coordinates
    vec2 ndcPos = vec2(
        (aPos.x * (maxPos.x - minPos.x) + minPos.x) / screenSize.x * 2.0 - 1.0,
        1.0 - (aPos.y * (maxPos.y - minPos.y) + minPos.y) / screenSize.y * 2.0
    );
    
    gl_Position = vec4(ndcPos, 0.0, 1.0);
    TexCoord = aPos;
} 