//
// Created by s5609424 on 20/05/25.
//

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
