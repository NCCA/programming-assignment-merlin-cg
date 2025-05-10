#version 330 core
uniform mat4 MVP;

layout (location=0) in vec3 inVert; // Vertex position (x, y, z)

out float outHeight; // Pass the original Y-coordinate (height) to fragment shader

void main()
{
    // Transform vertex position
    gl_Position = MVP * vec4(inVert, 1.0);
    
    // Pass the original y-coordinate of the vertex as height
    outHeight = inVert.y;
}

