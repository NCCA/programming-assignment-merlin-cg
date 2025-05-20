/*
 *Interface for terrain generation strategies, currently only uses PerlinNoiseGenerator
 */

#ifndef TERRAINGENERATOR_H
#define TERRAINGENERATOR_H

#include <vector>
#include <ngl/Vec3.h>

class TerrainGenerator {
public:
    virtual ~TerrainGenerator() = default;

    virtual void generateTerrain(std::vector<ngl::Vec3>& heightGrid, unsigned int width, unsigned int depth, float spacing, int maxHeight) = 0;

};

#endif //TERRAINGENERATOR_H
