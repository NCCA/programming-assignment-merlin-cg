#version 330 core
//HeightColourVertex.glsl
uniform mat4 MVP;
uniform float maxTerrainHeight;

layout (location=0) in vec3 inVert;
out float outHeight; // Pass the original Y-coordinate (height) to fragment shader
void main()
{
    gl_Position = MVP * vec4(inVert, 1.0);

    // Pass the original y-coordinate of the vertex as height
    outHeight = inVert.y;

}

