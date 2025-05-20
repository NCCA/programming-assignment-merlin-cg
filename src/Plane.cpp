/**
* Manages terrain mesh generation and rendering
 * Handles the creation, modification, and rendering of a 3D terrain mesh.
 * Uses the Strategy pattern for terrain generation and delegates erosion to HydraulicErosion.
 */


#include "Plane.h"
#include <iostream>
#include <ngl/Random.h>
#include <cmath>
#include <QOpenGLFunctions>
#include <ngl/VAOPrimitives.h>
#include <ngl/Mat4.h>
#include <ngl/ShaderLib.h>
#include <ngl/VAOFactory.h>
#include "PerlinNoise.hpp"
#include <random>
#include <ngl/Vec2.h>
#include "PerlinNoiseGenerator.h"

Plane::Plane(unsigned int _width, unsigned int _depth, float _spacing)
    : m_width(_width), m_depth(_depth), m_spacing(_spacing)
{
    // Initialize with default Perlin noise terrain generator
    m_terrainGenerator = std::make_shared<PerlinNoiseGenerator>(3.0f, 6, 90);
    generate();
}

void Plane::clearTerrainData()
{
    m_vertices.clear();
    m_heightGrid.clear();
}

void Plane::createBaseGridVertices()
{
    m_heightGrid.reserve(m_width * m_depth); // Reserve space for all unique vertices

    for (unsigned int z = 0; z < m_depth; ++z)
    {
        for (unsigned int x = 0; x < m_width; ++x)
        {
            float current_x_pos = x * m_spacing;
            float current_z_pos = z * m_spacing;
            // Y is 0.0f for the base grid; noise will be applied later.
            m_heightGrid.emplace_back(current_x_pos, 0.0f, current_z_pos);
        }
    }
}



void Plane::applyHydraulicErosion(int numDroplets, int dropletMaxLifetime) {
    // Delegate to the erosion object
    m_erosion.erode(m_heightGrid, m_width, m_depth, m_spacing, numDroplets, dropletMaxLifetime);

    // Update the mesh after erosion
    refreshGPUAssets();
}





void Plane::buildTriangleMeshFromGrid(const std::vector<ngl::Vec3>& noisyGridVertices)
{
    // Ensure m_vertices is clear before populating.
    m_vertices.clear();

    if (m_width < 2 || m_depth < 2) {
        std::cerr << "Plane::buildTriangleMeshFromGrid() - Cannot build mesh with width or depth < 2. m_vertices will be empty." << std::endl;
        return;
    }

    // Each grid cell becomes two triangles
    // Reserve space: (width-1) * (depth-1) * 2 triangles * 3 vertices per triangle
    m_vertices.reserve((m_width - 1) * (m_depth - 1) * 6);

    for (unsigned int z = 0; z < m_depth - 1; ++z)
    {
        for (unsigned int x = 0; x < m_width - 1; ++x)
        {
            // Get the indices of the four vertices forming the current quad from the 1D noisyGridVertices vector, which is ordered row by row.
            unsigned int topLeft = z * m_width + x;
            unsigned int topRight = topLeft + 1;
            unsigned int bottomLeft = (z + 1) * m_width + x;
            unsigned int bottomRight = bottomLeft + 1;

            // Triangle 1: topLeft, bottomLeft, topRight
            m_vertices.push_back(noisyGridVertices[topLeft]);
            m_vertices.push_back(noisyGridVertices[bottomLeft]);
            m_vertices.push_back(noisyGridVertices[topRight]);

            // Triangle 2: topRight, bottomLeft, bottomRight
            m_vertices.push_back(noisyGridVertices[topRight]);
            m_vertices.push_back(noisyGridVertices[bottomLeft]);
            m_vertices.push_back(noisyGridVertices[bottomRight]);
        }
    }
}

void Plane::setupTerrainVAO()
{
    // Always reset the VAO if it exists to free old GPU resources before creating/re-populating.
    if (m_vao)
    {
        m_vao.reset();
    }

    if (m_vertices.empty())
    {
        std::cerr << "Plane::setupTerrainVAO() - m_vertices is empty. Creating an empty VAO." << std::endl;
        m_vao = ngl::vaoFactoryCast<ngl::MultiBufferVAO>(
            ngl::VAOFactory::createVAO(ngl::multiBufferVAO, GL_TRIANGLES));
        m_vao->bind();
        m_vao->setNumIndices(0); // Set to 0 indices if no data
        m_vao->unbind();
        return;
    }

    m_vao = ngl::vaoFactoryCast<ngl::MultiBufferVAO>(
        ngl::VAOFactory::createVAO(ngl::multiBufferVAO, GL_TRIANGLES));

    m_vao->bind();

    m_vao->setData(ngl::MultiBufferVAO::VertexData(m_vertices.size() * sizeof(ngl::Vec3), m_vertices[0].m_x));

    m_vao->setVertexAttributePointer(0, 3, GL_FLOAT, 0, 0);

    m_vao->setNumIndices(m_vertices.size());

    m_vao->unbind();

}


void Plane::generate()
{

    clearTerrainData();

    createBaseGridVertices();
    m_erosion.clearDropletTrailPoints();
    m_terrainGenerator->generateTerrain(m_heightGrid, m_width, m_depth, m_spacing, m_maxHeight);

    buildTriangleMeshFromGrid(m_heightGrid);

    setupTerrainVAO();

    std::cout << "Plane::generate() - completed. Final m_vertices size for VAO: " << m_vertices.size() << std::endl;

}

void Plane::regenerate()
{
    generate(); // Re-run the generation logic
}

void Plane::refreshGPUAssets()
{
buildTriangleMeshFromGrid(m_heightGrid);
setupTerrainVAO();
}

void Plane::render() const
{
    if (!m_vao || m_vao->numIndices() == 0) { // Add a check to prevent drawing an invalid/empty VAO
        return;
    }


    m_vao->bind();
    //gl->glEnable(GL_PROGRAM_POINT_SIZE);
    m_vao->draw();
    //gl->glDisable(GL_POLYGON_OFFSET_FILL);
    //gl->glDisable(GL_PROGRAM_POINT_SIZE);
    m_vao->unbind();
}
