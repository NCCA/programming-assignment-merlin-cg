#ifndef PLANE_H
#define PLANE_H

#include <vector>
#include <ngl/Vec3.h>
#include <memory>
//#include <ngl/SimpleVAO.h>
#include <ngl/MultiBufferVAO.h>

struct GradientInfo {
    float dirX = 0.0f;
    float dirZ = 0.0f;
    float steepness = 0.0f;
};

class Plane
{
public:
    Plane(unsigned int _width, unsigned int _depth, float _spacing = 10.0f);
    void generate();
    void regenerate();
    void render() const;
    void setWidth(int width) {m_width = width; }
    int getWidth() const { return m_width; }

    void setDepth(int depth) {m_depth = depth; }
    int getDepth() const { return m_depth; }

    void applyPerlinNoise(float scale, float amplitude);
    void setNoiseFrequency(float freq) { m_noiseFrequency = freq; }
    float getNoiseFrequency() const { return m_noiseFrequency; }
    void setNoiseOctaves(int oct) { m_noiseOctaves = oct; }
    int getNoiseOctaves() const { return m_noiseOctaves; }

    //Erosion
    void applyHydraulicErosion();


    void setErosionIterations(int iterations) { m_erosionIterations = iterations; }
    void setNumErosionParticlesPerIteration(int numParticles) { m_numErosionDropletsPerIteration = numParticles; }
    void setDropletLifetime(int lifetime) { m_dropletLifetime = lifetime; }
    void setInertiaFactor(float inertia) { m_inertiaFactor = inertia; }
    void setSedimentCapacityFactor(float capacityFactor) { m_sedimentCapacityFactor = capacityFactor; }
    void setMinSedimentCapacity(float minCapacity) { m_minSedimentCapacity = minCapacity; }
    void setErosionRate(float rate) { m_erosionRate = rate; }
    void setDepositionRate(float rate) { m_depositionRate = rate; }
    void setEvaporationRate(float rate) { m_evaporationRate = rate; }
    void setGravity(float gravity) { m_gravity = gravity; }
    void setInitialWaterAmount(float water) { m_initialWaterAmount = water; }
    void setInitialSpeed(float speed) { m_initialSpeed = speed; }
    void setErosionRadius(float radius) { m_erosionRadius = radius; }
    void setDepositionRadius(float radius) { m_depositionRadius = radius; } // Added for symmetry with erosionRadius
    void setMaxErosionDepthFactor(float factor) { m_maxErosionDepthFactor = factor; }


private:
    // Helper methods for generation
    void clearTerrainData();
    void createBaseGridVertices();
    void applyPerlinNoiseToGrid();
    void buildTriangleMeshFromGrid(const std::vector<ngl::Vec3>& noisyGridVertices);
    void setupTerrainVAO();

    unsigned int m_width;
    unsigned int m_depth;
    std::vector<ngl::Vec3> m_verticesRaw; // grid vertices
    std::vector<ngl::Vec3> m_vertices;    // triangle vertices (duplicated)
    std::vector<GLuint> m_indices;
    std::vector<ngl::Vec3> m_heightGrid;
    float m_spacing;
    std::unique_ptr<ngl::MultiBufferVAO> m_vao;
    float m_noiseFrequency = 5.0f;
    int m_noiseOctaves = 4;

    // EROSION

    float getInterpolatedHeight(float worldX, float worldZ) const;
    GradientInfo calculateGradient(float worldX, float worldZ) const;

    // For distributing sediment/erosion, worldX/Z are center of effect, amount is total, radius is area of effect.
    void depositSedimentAt(float worldX, float worldZ, float amountToDeposit, float radius);
    void erodeTerrainAt(float worldX, float worldZ, float amountToErode, float radius);

    int m_erosionIterations = 30; // Default
    int m_numErosionDropletsPerIteration = 50; // Default
    int m_dropletLifetime = 30;         // Max steps for a droplet
    float m_inertiaFactor = 0.05f;      // How much previous direction is maintained (0-1)
    float m_sedimentCapacityFactor = 4.0f; // Multiplier for sediment capacity
    float m_minSedimentCapacity = 0.01f;  // A small minimum capacity
    float m_erosionRate = 0.3f;         // Factor for how much is eroded
    float m_depositionRate = 0.3f;      // Factor for how much is deposited
    float m_evaporationRate = 0.01f;    // Water lost per step
    float m_gravity = 4.0f;             // Affects particle acceleration/speed
    float m_initialWaterAmount = 1.0f;  // Starting water for a droplet
    float m_initialSpeed = 1.0f;        // Starting speed for a droplet
    float m_erosionRadius = 2.0f;       // Radius over which erosion is applied
    float m_depositionRadius = 2.0f;    // Radius over which deposition is applied
    float m_maxErosionDepthFactor = 0.1f; // Limits erosion per step relative to deltaHeight
};

#endif // PLANE_H
