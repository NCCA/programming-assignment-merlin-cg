#version 330 core
in vec3 vertColour;
layout (location=0)out vec4 outColour;
void main ()
{
 // set the fragment colour

    float dist = length(gl_PointCoord - vec2(0.5));

    // Discard pixels outside the radius for circular points
    if (dist > 0.5)
        discard;

 outColour = vec4(vertColour,1);
}
