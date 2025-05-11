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
    void applyPerlinNoise(float scale, float amplitude);
    void setFrequency(float freq) { m_noiseFrequency = freq; }
    float getFrequency() const { return m_noiseFrequency; }
    void setOctaves(int oct) { m_noiseOctaves = oct; }
    int getOctaves() const { return m_noiseOctaves; }

private:

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
