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

            float noiseFrequency = 5.0f;
            int noiseOctaves = 8;

            float height_normalized = perlin.octave2D_01(noiseInputX * noiseFrequency,
                                                         noiseInputZ * noiseFrequency,
                                                         noiseOctaves);

            float terrainMaxHeight = 8.0f; // Adjust as needed
            float y_pos = height_normalized * terrainMaxHeight;

            unique_vertices.emplace_back(current_x_pos, y_pos, current_z_pos);
        }
    }


    // Create a new vector that will hold vertices arranged for triangle drawing.
    std::vector<ngl::Vec3> render_vertices;
    // Reserve space: each quad becomes 2 triangles, each triangle has 3 vertices.
    if (m_width > 1 && m_depth > 1) {
        render_vertices.reserve((m_width - 1) * (m_depth - 1) * 6);
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
            render_vertices.push_back(unique_vertices[topLeft]);
            render_vertices.push_back(unique_vertices[bottomLeft]);
            render_vertices.push_back(unique_vertices[topRight]);

            // Triangle 2: topRight, bottomLeft, bottomRight
            render_vertices.push_back(unique_vertices[topRight]);
            render_vertices.push_back(unique_vertices[bottomLeft]);
            render_vertices.push_back(unique_vertices[bottomRight]);
        }
    }

    // Check if render_vertices is empty before proceeding
    if (render_vertices.empty()) {
        std::cerr << "Error: render_vertices is empty. No plane will be generated. Check with and depth" << std::endl;
        // Ensure m_vao is at least minimally valid if it's a member and accessed elsewhere
        if (!m_vao) { // Create it if it doesn't exist yet
            m_vao = ngl::vaoFactoryCast<ngl::SimpleVAO>(
                ngl::VAOFactory::createVAO(ngl::simpleVAO, GL_TRIANGLES));
        }
        m_vao->bind();
        m_vao->setNumIndices(0); // Set to 0 indices if no data
        m_vao->unbind();
        return;
    }

    // Set up VAO
    m_vao = ngl::vaoFactoryCast<ngl::SimpleVAO>(
        ngl::VAOFactory::createVAO(ngl::simpleVAO, GL_TRIANGLES)); // For now
    m_vao->bind();

    // Set the data using the render_vertices vector
    m_vao->setData(ngl::SimpleVAO::VertexData(render_vertices.size() * sizeof(ngl::Vec3), render_vertices[0].m_x));
    // Set the vertex attribute pointer for positions (attribute 0)
    m_vao->setVertexAttributePointer(0, 3, GL_FLOAT, 0, 0);
    m_vao->setNumIndices(render_vertices.size());

    m_vao->unbind();
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
