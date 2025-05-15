#version 330 core
//HeightColourFragment
in float outHeight;
layout (location=0) out vec4 fragColour;

uniform int maxTerrainHeight;
uniform float maxTerrainHeightFloat;

void main()
{
    float minTerrainHeight = 0.0;

    float normalizedHeight = clamp((outHeight - minTerrainHeight) / (maxTerrainHeight - minTerrainHeight), 0.0, 1.0);

    vec3 color = vec3(normalizedHeight, normalizedHeight, normalizedHeight);

    fragColour = vec4(color, 1.0);
}

