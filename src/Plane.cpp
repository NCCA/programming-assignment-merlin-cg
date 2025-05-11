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
static bool s_isFirstGeneration = true;

Plane::Plane(unsigned int _width, unsigned int _depth, float _spacing)
    : m_width(_width), m_depth(_depth), m_spacing(_spacing)
{
    generate();
}


void Plane::generate()
{

    std::cout << "Plane::generate() called. Using Frequency: " << m_noiseFrequency
              << ", Octaves: " << m_noiseOctaves << std::endl;


    const siv::PerlinNoise::seed_type seed = 123456u; // Seed
    const siv::PerlinNoise perlin{seed};
    m_vertices.clear();

    //Vector to store unique vertices on grid
    std::vector<ngl::Vec3> unique_vertices;
    unique_vertices.reserve(m_width * m_depth);

    float planeTotalWidth = (m_width -1) * m_spacing;
    float planeTotalDepth = (m_depth -1) * m_spacing;

    if (planeTotalWidth == 0.0f) planeTotalWidth = 1.0f; // Avoid division by zero
    if (planeTotalDepth == 0.0f) planeTotalDepth = 1.0f;

    for (unsigned int z = 0; z < m_depth; ++z)
    {
        for (unsigned int x = 0; x < m_width; ++x)
        {
            float current_x_pos = x * m_spacing;
            float current_z_pos = z * m_spacing;

            float noiseInputX = current_x_pos / planeTotalWidth;
            float noiseInputZ = current_z_pos / planeTotalDepth;

            // Ensure inputs are not problematic if m_width/m_depth is 1
            if (m_width == 1) noiseInputX = 0.0f;
            if (m_depth == 1) noiseInputZ = 0.0f;



            float height_normalized = perlin.octave2D_01(noiseInputX * m_noiseFrequency,
                                                         noiseInputZ * m_noiseFrequency,
                                                         m_noiseOctaves);

            float terrainMaxHeight = 8.0f; // Adjust as needed
            float y_pos = height_normalized * terrainMaxHeight;

            unique_vertices.emplace_back(current_x_pos, y_pos, current_z_pos);
        }
    }


    // Reserve space: each quad becomes 2 triangles, each triangle has 3 vertices.
    if (m_width > 1 && m_depth > 1) {
        m_vertices.reserve((m_width - 1) * (m_depth - 1) * 6);
    }


    std::vector<unsigned int> indices;
    for (int z = 0; z < m_depth - 1; ++z) {
        for (int x = 0; x < m_width - 1; ++x) {
            // Get the indices of the four vertices forming the current quad
            unsigned int topLeft = z * m_width + x;
            unsigned int topRight = topLeft + 1;
            unsigned int bottomLeft = (z + 1) * m_width + x;
            unsigned int bottomRight = bottomLeft + 1;


            // Triangle 1: topLeft, bottomLeft, topRight
            m_vertices.push_back(unique_vertices[topLeft]);
            m_vertices.push_back(unique_vertices[bottomLeft]);
            m_vertices.push_back(unique_vertices[topRight]);

            // Triangle 2: topRight, bottomLeft, bottomRight
            m_vertices.push_back(unique_vertices[topRight]);
            m_vertices.push_back(unique_vertices[bottomLeft]);
            m_vertices.push_back(unique_vertices[bottomRight]);
        }
    }

    if (m_vertices.empty()) {
        std::cerr << "Error: m_vertices is empty. No plane will be generated. Check with and depth" << std::endl;
        // Ensure m_vao is at least minimally valid if it's a member and accessed elsewhere



        if (m_vertices.empty()) { /* ... handle empty ... */ return; }

        m_vao.reset(); // <<< THIS LINE HERE
        m_vao = ngl::vaoFactoryCast<ngl::MultiBufferVAO>(
            ngl::VAOFactory::createVAO(ngl::multiBufferVAO,GL_TRIANGLES));

        m_vao->bind();
        m_vao->setNumIndices(0); // Set to 0 indices if no data
        m_vao->unbind();
        return;
    }

    // If VAO doesnt exist, create it. Otherwise, we assume we are re-populating it.
    m_vao = ngl::vaoFactoryCast<ngl::MultiBufferVAO>(
        ngl::VAOFactory::createVAO(ngl::multiBufferVAO, GL_TRIANGLES)); // For now
    m_vao->bind();

    // Set the data using the render_vertices vector
    m_vao->setData(ngl::MultiBufferVAO::VertexData(m_vertices.size() * sizeof(ngl::Vec3), m_vertices[0].m_x));
    // Set the vertex attribute pointer for positions (attribute 0)
    m_vao->setVertexAttributePointer(0, 3, GL_FLOAT, 0, 0);
    m_vao->setNumIndices(m_vertices.size());


    std::cout << "Plane::generate() - unique_vertices size: " << unique_vertices.size() << std::endl;
    std::cout << "Plane::generate() - m_vertices (render_vertices) size: " << m_vertices.size() << std::endl;
    if (!m_vertices.empty()) {
        std::cout << "First vertex in m_vertices: (" << m_vertices[0].m_x << ", "
                  << m_vertices[90].m_y << ", " << m_vertices[90].m_z << ")" << std::endl;
    }


    m_vao->unbind();
}

void Plane::regenerate()
{
    std::cout << "Plane::regenerate() called. Frequency: " << m_noiseFrequency << ", Octaves: " << m_noiseOctaves << std::endl;
    generate(); // Re-run the generation logic

}

void Plane::render() const
{
  //  auto *gl = QOpenGLContext::currentContext()->functions();
    if (!m_vao || m_vao->numIndices() == 0) { // Add a check to prevent drawing an invalid/empty VAO
        return;
    }
    m_vao->bind();
    //gl->glEnable(GL_PROGRAM_POINT_SIZE);
    m_vao->draw();
    //gl->glDisable(GL_PROGRAM_POINT_SIZE);
    m_vao->unbind();
}
