#include "Plane.h"
#include <iostream>
#include <ngl/Random.h>
#include <algorithm>
#include <fstream>
#include <cmath>
#include <QOpenGLFunctions>
#include <ngl/VAOPrimitives.h>
#include <ngl/Mat4.h>
#include <ngl/ShaderLib.h>
#include <ngl/Transformation.h>
#include <ngl/Util.h>
#include <ngl/VAOFactory.h>
#include "PerlinNoise.hpp"

Plane::Plane(unsigned int _width, unsigned int _depth, float _spacing)
    : m_width(_width), m_depth(_depth), m_spacing(_spacing)
{
    generate();
}


void Plane::generate()
{
    const siv::PerlinNoise::seed_type seed = 123456u; // Seed
    const siv::PerlinNoise perlin{seed};
    m_vertices.clear();

    float planeTotalWidth = (m_width -1) * m_spacing;
    float planeTotalDepth = (m_depth -1) * m_spacing;

    for (unsigned int z = 0; z < m_depth; ++z)
    {
        for (unsigned int x = 0; x < m_width; ++x)
        {
            float current_x_pos = x * m_spacing;
            float current_z_pos = z * m_spacing;

            float noiseInputX = current_x_pos / planeTotalWidth;
            float noiseInputZ = current_z_pos / planeTotalDepth;

            if (m_width <= 1) noiseInputX = 0.0f;
            if (m_depth <= 1) noiseInputZ = 0.0f;

            float noiseFrequency = 5.0f;
            int noiseOctaves = 4;


            float height_normalized = perlin.octave2D_01(noiseInputX * noiseFrequency,
                                                         noiseInputZ * noiseFrequency,
                                                         noiseOctaves);


            float terrainMaxHeight = 8.0f;
            float y_pos = height_normalized * terrainMaxHeight;

            m_vertices.emplace_back(current_x_pos, y_pos, current_z_pos);

        }
    }

    // Set up VAO
    m_vao = ngl::vaoFactoryCast<ngl::SimpleVAO>(
        ngl::VAOFactory::createVAO(ngl::simpleVAO, GL_POINTS)); // For now
    m_vao->bind();
    m_vao->setData(ngl::SimpleVAO::VertexData(
        m_vertices.size() * sizeof(ngl::Vec3), m_vertices[0].m_x));
    m_vao->setVertexAttributePointer(0, 3, GL_FLOAT, 0, 0);
    m_vao->setNumIndices(m_vertices.size());
    m_vao->unbind();
}



void Plane::render() const
{
    auto *gl = QOpenGLContext::currentContext()->functions();

    m_vao->bind();
    gl->glEnable(GL_PROGRAM_POINT_SIZE);
    m_vao->draw();
    gl->glDisable(GL_PROGRAM_POINT_SIZE);
    m_vao->unbind();
}
