#version 330 core
/// @brief Model View Projection Matrix
uniform mat4 MVP;

layout (location=0)in vec3 inVert;
layout (location=1)in vec4 inColour;
out vec4 vertColour;

void main()
{

 // calculate the vertex position
 gl_Position = MVP*vec4(inVert, 1.0);
 vertColour=inColour;
}











