#version 410 core

layout (location=0) out vec4 fragColour;
in vec3 particle_colour;
void main()
{
    vec2 circle_coord = 2.0 * gl_PointCoord - 1.0;
    if(dot(circle_coord,circle_coord) > 1.0)
    {
        discard;
    }
    fragColour=vec4(particle_colour,1.0);
}
