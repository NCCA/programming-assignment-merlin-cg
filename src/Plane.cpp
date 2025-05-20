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
#include <random>
#include <ngl/Vec2.h>
#include "DropletVisualize.h"
#include "PerlinNoiseGenerator.h"

static bool s_isFirstGeneration = true;
bool outDebugStatements = false;

Plane::Plane(unsigned int _width, unsigned int _depth, float _spacing)
    : m_width(_width), m_depth(_depth), m_spacing(_spacing)
{
    // Create a default terrain generator (PerlinNoiseGenerator)
    m_terrainGenerator = std::make_shared<PerlinNoiseGenerator>(3.0f, 6, 90);
    generate();
}

void Plane::clearTerrainData()
{
    //std::cout << "Plane::clearTerrainData() called." << std::endl;
    m_vertices.clear();
    m_heightGrid.clear();
}

void Plane::createBaseGridVertices()
{
    //std::cout << "Plane::createBaseGridVertices() called." << std::endl;
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
    //std::cout << "Plane::createBaseGridVertices() - generated " << m_heightGrid.size() << " base vertices." << std::endl;
}

void Plane::applyPerlinNoiseToGrid()
{
    //std::cout << "Plane::applyPerlinNoiseToGrid() called for member m_heightGrid." << std::endl;
    if (this->m_heightGrid.empty())
    {
        std::cerr << "Error: m_heightGrid is empty in applyPerlinNoiseToGrid(). Cannot apply noise." << std::endl;
        return;
    }

    const siv::PerlinNoise::seed_type seed = 123456u;
    const siv::PerlinNoise perlin{seed};
    float planeTotalWidth = (m_width > 1) ? (m_width - 1) * m_spacing : 1.0f;
    float planeTotalDepth = (m_depth > 1) ? (m_depth - 1) * m_spacing : 1.0f;
    if (planeTotalWidth == 0.0f) planeTotalWidth = 1.0f;
    if (planeTotalDepth == 0.0f) planeTotalDepth = 1.0f;

    for (ngl::Vec3& vertex : this->m_heightGrid) // Iterate by reference to modify Y values
    {
        float current_x_pos = vertex.m_x;
        float current_z_pos = vertex.m_z;
        float noiseInputX = (m_width == 1) ? 0.0f : current_x_pos / planeTotalWidth;
        float noiseInputZ = (m_depth == 1) ? 0.0f : current_z_pos / planeTotalDepth;
        float height_normalized = std::abs(perlin.octave2D_01(noiseInputX * m_noiseFrequency,
                                                     noiseInputZ * m_noiseFrequency,
                                                                     m_noiseOctaves, 0.5));
        vertex.m_y = height_normalized * m_maxHeight;
    }
    //std::cout << "Plane::applyPerlinNoiseToGrid() - applied noise to " << this->m_heightGrid.size() << " vertices in member m_heightGrid." << std::endl;
}


void Plane::applyHydraulicErosion(int numDroplets, int dropletMaxLifetime) {
    // Delegate to the erosion object
    m_erosion.erode(m_heightGrid, m_width, m_depth, m_spacing, numDroplets, dropletMaxLifetime);

    // Update the mesh after erosion
    refreshGPUAssets();
}





void Plane::buildTriangleMeshFromGrid(const std::vector<ngl::Vec3>& noisyGridVertices)
{
    //std::cout << "Plane::buildTriangleMeshFromGrid() called." << std::endl;

    // Ensure m_vertices is clear before populating.
    m_vertices.clear();

    if (m_width < 2 || m_depth < 2) {
        std::cerr << "Plane::buildTriangleMeshFromGrid() - Cannot build mesh with width or depth < 2. m_vertices will be empty." << std::endl;
        return;
    }

    // Reserve space: each quad becomes 2 triangles, each triangle has 3 vertices.
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
    //std::cout << "Plane::buildTriangleMeshFromGrid() - populated m_vertices with " << m_vertices.size() << " vertices for rendering." << std::endl;
}

void Plane::setupTerrainVAO()
{
    //std::cout << "Plane::setupTerrainVAO() called." << std::endl;

    // Always reset the VAO if it exists to free old GPU resources before creating/re-populating.
    if (m_vao)
    {
        m_vao.reset();
    }

    if (m_vertices.empty())
    {
        std::cerr << "Plane::setupTerrainVAO() - m_vertices is empty. Creating an empty VAO." << std::endl;
        // Create a new, empty VAO to avoid issues with an uninitialized or stale m_vao.
        // This ensures render() can safely check m_vao->numIndices() == 0.
        m_vao = ngl::vaoFactoryCast<ngl::MultiBufferVAO>(
            ngl::VAOFactory::createVAO(ngl::multiBufferVAO, GL_TRIANGLES));
        m_vao->bind();
        m_vao->setNumIndices(0); // Set to 0 indices if no data
        m_vao->unbind();
        return; // Exit if no vertices to upload
    }

    m_vao = ngl::vaoFactoryCast<ngl::MultiBufferVAO>(
        ngl::VAOFactory::createVAO(ngl::multiBufferVAO, GL_TRIANGLES));

    m_vao->bind();

    m_vao->setData(ngl::MultiBufferVAO::VertexData(m_vertices.size() * sizeof(ngl::Vec3), m_vertices[0].m_x));

    m_vao->setVertexAttributePointer(0, 3, GL_FLOAT, 0, 0);

    m_vao->setNumIndices(m_vertices.size());

    m_vao->unbind();

    //std::cout << "Plane::setupTerrainVAO() - VAO configured with " << m_vertices.size() << " vertices." << std::endl;
}




void Plane::generate()
{

    clearTerrainData();

    createBaseGridVertices();
    m_erosion.clearDropletTrailPoints();
    m_terrainGenerator->generateTerrain(m_heightGrid, m_width, m_depth, m_spacing, m_maxHeight);

    buildTriangleMeshFromGrid(m_heightGrid);

    setupTerrainVAO();

    //std::cout << "Plane::generate() - completed. Final m_vertices size for VAO: " << m_vertices.size() << std::endl;

}

void Plane::regenerate()
{
    //std::cout << "Plane::regenerate() called. Frequency: " << m_noiseFrequency << ", Octaves: " << m_noiseOctaves << std::endl;
    generate(); // Re-run the generation logic
}

void Plane::refreshGPUAssets()
{
buildTriangleMeshFromGrid(m_heightGrid);
setupTerrainVAO();
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
    //gl->glDisable(GL_POLYGON_OFFSET_FILL);
    //gl->glDisable(GL_PROGRAM_POINT_SIZE);
    m_vao->unbind();
}
