//
// Created by s5609424 on 20/05/25.
//

#ifndef HYDRAULICEROSION_H
#define HYDRAULICEROSION_H

#include <vector>
#include <ngl/Vec2.h>
#include <ngl/Vec3.h>
#include <ngl/Vec4.h>

struct HeightAndGradientData {
    float height = 0.0f;
    ngl::Vec2 rawGradientAscent{0.0f, 0.0f};
};

class HydraulicErosion {
public:
    HydraulicErosion();

    // Main method to perform erosion on a height grid
    void erode(std::vector<ngl::Vec3>& heightGrid,
               unsigned int width,
               unsigned int depth,
               float spacing,
               int numDroplets,
               int dropletMaxLifetime);

    // Getters/setters for erosion parameters
    void setErosionRate(float rate) { m_erosionRate = rate; }
    float getErosionRate() const { return m_erosionRate; }

    void setDepositionRate(float rate) { m_depositionRate = rate; }
    float getDepositionRate() const { return m_depositionRate; }

    // Additional parameter getters/setters...

    // Access to visualization data
    const std::vector<ngl::Vec4>& getDropletTrailPoints() const { return m_dropletTrailPoints; }

private:
    // Helper methods
    HeightAndGradientData getHeightAndGradient(const std::vector<ngl::Vec3>& heightGrid,
                                              unsigned int width,
                                              unsigned int depth,
                                              float spacing,
                                              float worldX,
                                              float worldZ) const;

    void computeAreaOfInfluence(unsigned int width, unsigned int depth, float radius);

    // Droplet structure
    struct Droplet {
        ngl::Vec2 pos;
        ngl::Vec2 dir;
        float speed;
        float water;
        float sediment;
        int lifetime;

        Droplet(ngl::Vec2 startPos, float initialSpeed, float initialWater, int maxLifetime)
            : pos(startPos), dir(0.0f, 0.0f), speed(initialSpeed),
              water(initialWater), sediment(0.0f), lifetime(maxLifetime) {}
    };

    // Erosion parameters
    float m_inertiaFactor = 0.05f;
    float m_sedimentCapacityFactor = 4.0f;
    float m_minSedimentCapacity = 0.01f;
    float m_erosionRate = 0.2f;
    float m_depositionRate = 0.1f;
    float m_evaporationRate = 0.1f;
    float m_gravity = 4.0f;
    float m_initialWaterAmount = 1.0f;
    float m_initialSpeed = 1.0f;
    int m_erosionRadius = 3;
    float m_depositionRadius = 3.0;
    float m_maxErosionDepthFactor = 0.5f;
    float m_friction = 0.0f;

    // Data structures
    std::vector<ngl::Vec4> m_dropletTrailPoints;
    std::vector<std::vector<int>> m_brushIndices;
    std::vector<std::vector<float>> m_brushWeights;
};


#endif //HYDRAULICEROSION_H
