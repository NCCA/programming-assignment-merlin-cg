#ifndef PLANE_H
#define PLANE_H

#include <vector>
#include <ngl/Vec3.h>
#include <memory>
//#include <ngl/SimpleVAO.h>
#include <ngl/MultiBufferVAO.h>
#include <functional>
#include <ngl/Vec2.h>

struct HeightAndGradientData {
    float height = 0.0f;
    ngl::Vec2 rawGradientAscent{0.0f, 0.0f}; // Gradient components pointing towards steepest ASCENT
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
    void setTerrainHeight(int height) { m_maxHeight = height; }
    int getTerrainHeight() const { return m_maxHeight; }
    void refreshGPUAssets();



    //Erosion
    void applyHydraulicErosion(int numDroplets, int dropletMaxLifetime /*, other params */);
    const std::vector<ngl::Vec4>& getDropletTrailPoints() const { return m_dropletTrailPoints; }

private:
    // Helper methods for generation
    void clearTerrainData();
    void createBaseGridVertices();
    void applyPerlinNoiseToGrid();
    void buildTriangleMeshFromGrid(const std::vector<ngl::Vec3>& noisyGridVertices);
    void setupTerrainVAO();
    void smoothTerrain(float);
    void computeAreaOfInfluence(float radius);
    void updateDropVisualisation(bool toggle);
    unsigned int m_width;
    unsigned int m_depth;
    std::vector<ngl::Vec3> m_verticesRaw; // grid vertices
    std::vector<ngl::Vec3> m_vertices;    // triangle vertices (duplicated)
    std::vector<GLuint> m_indices;
    std::vector<ngl::Vec3> m_heightGrid;
    float m_spacing;
    std::unique_ptr<ngl::MultiBufferVAO> m_vao;
    float m_noiseFrequency = 1.0f;
    int m_noiseOctaves = 10;
    int m_maxHeight = 30;

    // EROSION
    HeightAndGradientData getHeightAndGradient(float worldX, float worldZ) const;
    std::vector<ngl::Vec4> m_dropletTrailPoints;
    std::vector<std::vector<int>> m_brushIndices;
    std::vector<std::vector<float>> m_brushWeights;


    struct Droplet {
        ngl::Vec2 pos;          // Current 2D position (world coordinates)
        ngl::Vec2 dir;          // Current 2D direction of movement (normalized)
        float speed;            // Current speed of the droplet
        float water;            // Amount of water in the droplet
        float sediment;         // Amount of sediment carried by the droplet
        int lifetime;           // Remaining steps for the droplet to live

        // Constructor for easy initialization
        Droplet(ngl::Vec2 startPos, float initialSpeed, float initialWater, int maxLifetime)
            : pos(startPos), dir(0.0f, 0.0f), speed(initialSpeed),
            water(initialWater), sediment(0.0f), lifetime(maxLifetime) {}
    };

    int m_erosionIterations = 15000; // Default
    int m_numDropletsPerIteration = 70; // Default
    int m_dropletLifetime = 30;         // Max steps for a droplet
    float m_inertiaFactor = 0.05f;      // How much previous direction is maintained (0-1)
    float m_sedimentCapacityFactor = 4.0f; // Multiplier for sediment capacity
    float m_minSedimentCapacity = 0.01f;  // A small minimum capacity
    float m_erosionRate = 0.2f;         // Factor for how much is eroded
    float m_depositionRate = 0.1f;      // Factor for how much is deposited
    float m_evaporationRate = 0.1f;    // Water lost per step
    float m_gravity = 4.0f;             // Affects particle acceleration/speed
    float m_initialWaterAmount = 1.0f;  // Starting water for a droplet
    float m_initialSpeed = 1.0f;        // Starting speed for a droplet
    int m_erosionRadius = 3;       // Radius over which erosion is appliede
    float m_depositionRadius = 3.0;    // Radius over which deposition is applied
    float m_maxErosionDepthFactor = 0.5f; // Limits erosion per step relative to deltaHeight
    float m_friction = 0.0f;
};

#endif // PLANE_H
