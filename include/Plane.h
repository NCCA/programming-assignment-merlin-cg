#ifndef PLANE_H
#define PLANE_H

#include <vector>
#include <ngl/Vec3.h>
#include <memory>
#include <ngl/MultiBufferVAO.h>
#include <ngl/Vec2.h>
#include "HydraulicErosion.h"


class Plane
{
public:
    Plane(unsigned int _width, unsigned int _depth, float _spacing = 10.0f);
    void generate();
    void regenerate();
    void render() const;
    void refreshGPUAssets();

    void setWidth(int width) {m_width = width; }
    int getWidth() const { return m_width; }
    void setDepth(int depth) {m_depth = depth; }
    int getDepth() const { return m_depth; }
    void setNoiseFrequency(float freq) { m_noiseFrequency = freq; }
    float getNoiseFrequency() const { return m_noiseFrequency; }
    void setNoiseOctaves(int oct) { m_noiseOctaves = oct; }
    int getNoiseOctaves() const { return m_noiseOctaves; }
    void setTerrainHeight(int height) { m_maxHeight = height; }
    int getTerrainHeight() const { return m_maxHeight; }

    //Erosion
    void applyHydraulicErosion(int numDroplets, int dropletMaxLifetime /*, other params */);
    const std::vector<ngl::Vec4>& getDropletTrailPoints() const { return m_erosion.getDropletTrailPoints(); }
    //const HydraulicErosion& getErosion() const { return m_erosion; }
private:
    // Helper methods for generation
    void clearTerrainData();
    void createBaseGridVertices();
    void applyPerlinNoiseToGrid();
    void buildTriangleMeshFromGrid(const std::vector<ngl::Vec3>& noisyGridVertices);
    void setupTerrainVAO();
    void computeAreaOfInfluence(float radius);
    unsigned int m_width;
    unsigned int m_depth;
    std::vector<ngl::Vec3> m_verticesRaw; // grid vertices
    std::vector<ngl::Vec3> m_vertices;    // triangle vertices (duplicated)
    std::vector<GLuint> m_indices;
    std::vector<ngl::Vec3> m_heightGrid;
    float m_spacing;
    std::unique_ptr<ngl::MultiBufferVAO> m_vao;
    float m_noiseFrequency = 3.0f;
    int m_noiseOctaves = 6;
    int m_maxHeight = 90;

    // EROSION
    //HeightAndGradientData getHeightAndGradient(float worldX, float worldZ) const;
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


    HydraulicErosion m_erosion;
};

#endif // PLANE_H
