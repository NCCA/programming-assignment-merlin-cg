#version 330 core
//HeightColourFragment
in float outHeight; // Height received from vertex shader (original y-coordinate)

layout (location=0) out vec4 fragColour;

void main()
{
    float minTerrainHeight = 0.0;
    float maxTerrainHeight = 8.0; 

    float normalizedHeight = clamp((outHeight - minTerrainHeight) / (maxTerrainHeight - minTerrainHeight), 0.0, 1.0);

    vec3 color = vec3(normalizedHeight, normalizedHeight, normalizedHeight);

    fragColour = vec4(color, 1.0);
}

