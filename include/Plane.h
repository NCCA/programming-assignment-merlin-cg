#ifndef PLANE_H
#define PLANE_H

#include <vector>
#include <ngl/Vec3.h>
#include <memory>
//#include <ngl/SimpleVAO.h>
#include <ngl/MultiBufferVAO.h>

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

private:
    // Helper methods for refactored generation
    void clearTerrainData();
    std::vector<ngl::Vec3> createBaseGridVertices();
    void applyPerlinNoiseToGrid(std::vector<ngl::Vec3>& gridVertices);
    void buildTriangleMeshFromGrid(const std::vector<ngl::Vec3>& noisyGridVertices);
    void setupTerrainVAO();

    unsigned int m_width;
    unsigned int m_depth;
    std::vector<ngl::Vec3> m_verticesRaw; // grid vertices
    std::vector<ngl::Vec3> m_vertices;    // triangle vertices (duplicated)
    std::vector<GLuint> m_indices;
    float m_spacing;
    std::unique_ptr<ngl::MultiBufferVAO> m_vao;
    float m_noiseFrequency = 5.0f;
    int m_noiseOctaves = 4;
};

#endif // PLANE_H
