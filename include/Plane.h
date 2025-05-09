#ifndef PLANE_H
#define PLANE_H

#include <vector>
#include <ngl/Vec3.h>
#include <memory>
#include <ngl/SimpleVAO.h>

class Plane
{
public:
    Plane(unsigned int _width, unsigned int _depth, float _spacing = 10.0f);
    void generate();
    void render() const;
    void applyPerlinNoise(float scale, float amplitude);

private:

    unsigned int m_width;
    unsigned int m_depth;
    std::vector<ngl::Vec3> m_verticesRaw; // grid vertices
    std::vector<ngl::Vec3> m_vertices;    // triangle vertices (duplicated)
    std::vector<GLuint> m_indices;
    float m_spacing;
    std::unique_ptr<ngl::SimpleVAO> m_vao;
};

#endif // PLANE_H
