//
// Created by s5609424 on 20/05/25.
//

#ifndef PERLINNOISEGENERATOR_H
#define PERLINNOISEGENERATOR_H
#include "TerrainGenerator.h"

class PerlinNoiseGenerator : public TerrainGenerator {
public:
    PerlinNoiseGenerator(float frequency = 3.0f, int octaves = 6, int maxHeight = 90);

    void generateTerrain(std::vector<ngl::Vec3>& heightGrid, unsigned int width, unsigned int depth, float spacing, int m_maxHeight) override;



    // Parameter setters/getters
    void setFrequency(float freq) { m_frequency = freq; }
    float getFrequency() const { return m_frequency; }

    void setOctaves(int oct) { m_octaves = oct; }
    int getOctaves() const { return m_octaves; }

    int getMaxHeight() const { return m_maxHeight; }

private:
    float m_frequency;
    int m_octaves;
    int m_maxHeight;
};
#endif //PERLINNOISEGENERATOR_H
