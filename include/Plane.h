#ifndef PLANE_H
#define PLANE_H

#include <vector>
#include <ngl/Vec3.h>
#include <memory>
#include <ngl/MultiBufferVAO.h>
#include <ngl/Vec2.h>
#include "HydraulicErosion.h"
#include "TerrainGenerator.h"
#include "PerlinNoiseGenerator.h"

/**
 * Manages terrain mesh generation and rendering
 * Handles the creation, modification, and rendering of a 3D terrain mesh.
 * Uses the Strategy pattern for terrain generation and delegates erosion to HydraulicErosion.
 */

class Plane
{
public:
    Plane(unsigned int _width, unsigned int _depth, float _spacing = 10.0f);
    void generate();
    void regenerate();
    void render() const;
    void refreshGPUAssets();
    void setTerrainGenerator(std::shared_ptr<TerrainGenerator> generator) {
        m_terrainGenerator = generator;
    }

    void setWidth(int width) {m_width = width; }
    int getWidth() const { return m_width; }
    void setDepth(int depth) {m_depth = depth; }
    int getDepth() const { return m_depth; }
    float getNoiseFrequency() const { return m_noiseFrequency; }
    int getNoiseOctaves() const { return m_noiseOctaves; }
    void setTerrainHeight(int height) { m_maxHeight = height; }
    int getTerrainHeight() const { return m_maxHeight; }

    /**
 * Updates noise frequency and propagates to terrain generator
 * Changes take effect on next regenerate() call
 */
    void setNoiseFrequency(float freq) {
        m_noiseFrequency = freq;
        auto perlinGen = std::dynamic_pointer_cast<PerlinNoiseGenerator>(m_terrainGenerator);
        if(perlinGen) perlinGen->setFrequency(freq);
    }

    /**
 * Updates noise frequency and propagates to terrain generator
 * Changes take effect on next regenerate() call
 */
    void setNoiseOctaves(int oct) {
        m_noiseOctaves = oct;
        auto perlinGen = std::dynamic_pointer_cast<PerlinNoiseGenerator>(m_terrainGenerator);
        if(perlinGen) perlinGen->setOctaves(oct);
    }

    //Erosion
    void applyHydraulicErosion(int numDroplets, int dropletMaxLifetime /*, other params */);
    // Delegate access to droplet trailpoitns
    const std::vector<ngl::Vec4>& getDropletTrailPoints() const { return m_erosion.getDropletTrailPoints(); }
private:

    // Helper methods for generation
    void clearTerrainData();
    void createBaseGridVertices();
    void buildTriangleMeshFromGrid(const std::vector<ngl::Vec3>& noisyGridVertices);
    void setupTerrainVAO();


    unsigned int m_width;
    unsigned int m_depth;
    std::vector<ngl::Vec3> m_verticesRaw; // grid vertices
    std::vector<ngl::Vec3> m_vertices;    // triangle vertices (duplicated)
    std::vector<GLuint> m_indices;
    std::vector<ngl::Vec3> m_heightGrid;
    float m_spacing;

    // Rendering
    std::unique_ptr<ngl::MultiBufferVAO> m_vao;


    float m_noiseFrequency = 3.0f;
    int m_noiseOctaves = 6;
    int m_maxHeight = 90;

    std::shared_ptr<TerrainGenerator> m_terrainGenerator;

    HydraulicErosion m_erosion;
};

#endif // PLANE_H
