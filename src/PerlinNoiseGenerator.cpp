//THIS USES A HEADER LIBRARY https://github.com/Reputeless/PerlinNoise


#include "PerlinNoiseGenerator.h"
#include "PerlinNoise.hpp"

PerlinNoiseGenerator::PerlinNoiseGenerator(float frequency, int octaves, int maxHeight)
    : m_frequency(frequency), m_octaves(octaves), m_maxHeight(maxHeight)
{
    // Constructor implementation
}

void PerlinNoiseGenerator::generateTerrain(std::vector<ngl::Vec3>& heightGrid,
                                          unsigned int width,
                                          unsigned int depth,
                                          float spacing, int maxHeight)
{
    if (heightGrid.empty()) {
        return;
    }

    const siv::PerlinNoise::seed_type seed = 123456u;
    const siv::PerlinNoise perlin{seed};
    float planeTotalWidth = (width > 1) ? (width - 1) * spacing : 1.0f;
    float planeTotalDepth = (depth > 1) ? (depth - 1) * spacing : 1.0f;
    if (planeTotalWidth == 0.0f) planeTotalWidth = 1.0f;
    if (planeTotalDepth == 0.0f) planeTotalDepth = 1.0f;

    for (ngl::Vec3& vertex : heightGrid)
    {
        float current_x_pos = vertex.m_x;
        float current_z_pos = vertex.m_z;
        float noiseInputX = (width == 1) ? 0.0f : current_x_pos / planeTotalWidth;
        float noiseInputZ = (depth == 1) ? 0.0f : current_z_pos / planeTotalDepth;
        float height_normalized = std::abs(perlin.octave2D_01(noiseInputX * m_frequency,
                                                   noiseInputZ * m_frequency,
                                                   m_octaves, 0.5));
        vertex.m_y = height_normalized * maxHeight;
    }
}