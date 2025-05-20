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
#include "Emitter.h"

static bool s_isFirstGeneration = true;
bool outDebugStatements = false;

Plane::Plane(unsigned int _width, unsigned int _depth, float _spacing)
    : m_width(_width), m_depth(_depth), m_spacing(_spacing)
{
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

HeightAndGradientData Plane::getHeightAndGradient(float worldX, float worldZ) const
{
    HeightAndGradientData result;

    float gridFloatX = worldX / m_spacing;
    float gridFloatZ = worldZ / m_spacing;

    int coordX = static_cast<int>(std::floor(gridFloatX));
    int coordZ = static_cast<int>(std::floor(gridFloatZ));

    // Calculate droplet's offset inside the cell (0,0) = at NW node, (1,1) = at SE node
    float offsetX = gridFloatX - coordX;
    float offsetZ = gridFloatZ - coordZ;

    // Define and clamp coordinates for the four cell corners
    int x0 = coordX;
    int z0 = coordZ;
    int x1 = coordX + 1;
    int z1 = coordZ + 1;
    int c_x0 = std::clamp(x0, 0, static_cast<int>(m_width) - 1);
    int c_z0 = std::clamp(z0, 0, static_cast<int>(m_depth) - 1);
    int c_x1 = std::clamp(x1, 0, static_cast<int>(m_width) - 1);
    int c_z1 = std::clamp(z1, 0, static_cast<int>(m_depth) - 1);

    // Calculate heights of the four nodes of the droplet's cell
    // index = z * m_width + x
    float hNW = m_heightGrid[c_z0 * m_width + c_x0].m_y;
    float hNE = m_heightGrid[c_z0 * m_width + c_x1].m_y;
    float hSW = m_heightGrid[c_z1 * m_width + c_x0].m_y;
    float hSE = m_heightGrid[c_z1 * m_width + c_x1].m_y;

    // Calculate droplet's direction of flow (gradient) with bilinear interpolation of height difference along the edges
    // This is the gradient of ascent
    result.rawGradientAscent.m_x = (hNE - hNW) * (1.0f - offsetZ) + (hSE - hSW) * offsetZ;
    result.rawGradientAscent.m_y = (hSW - hNW) * (1.0f - offsetX) + (hSE - hNE) * offsetX; // m_y for Z-gradient

    // Calculate height with bilinear interpolation of the heights of the nodes of the cell
    float height_lerp_bottom = hNW * (1.0f - offsetX) + hNE * offsetX;
    float height_lerp_top    = hSW * (1.0f - offsetX) + hSE * offsetX;
    result.height = height_lerp_bottom * (1.0f - offsetZ) + height_lerp_top * offsetZ;
    
    return result;

}

// Inspired by Sebastian Lague's hydraulic erosion implementation
void Plane::applyHydraulicErosion(int numDroplets, int dropletMaxLifetime)
{

    if (m_heightGrid.empty()) { /* ... error handling ... */ return; }
    computeAreaOfInfluence(m_erosionRadius);
   // m_dropletTrailPoints.clear(); // Clear previous trails before starting a new simulation
   // m_dropletTrailPoints.reserve(numDroplets * dropletMaxLifetime);


    for (int i = 0; i < numDroplets; ++i) {
        // Initialize Droplet (as per previous snippets)
        // ... (startX, startZ, create droplet instance)
        int randGridX = ngl::Random::randomPositiveNumber(m_width - 1);
        int randGridZ = ngl::Random::randomPositiveNumber(m_depth - 1);

        float startX = static_cast<float>(randGridX) * m_spacing;
        float startZ = static_cast<float>(randGridZ) * m_spacing;
        Droplet droplet(ngl::Vec2(startX, startZ), m_initialSpeed, m_initialWaterAmount, dropletMaxLifetime);

        for (int step = 0; step < dropletMaxLifetime; ++step)
        {

            HeightAndGradientData hgDataOld = getHeightAndGradient(droplet.pos.m_x, droplet.pos.m_y);
            float originalTerrainHeight = hgDataOld.height;

            droplet.dir.m_x = (droplet.dir.m_x * m_inertiaFactor - hgDataOld.rawGradientAscent.m_x * (1 - m_inertiaFactor));
            droplet.dir.m_y = (droplet.dir.m_y * m_inertiaFactor - hgDataOld.rawGradientAscent.m_y * (1 - m_inertiaFactor));

            float length = std::sqrt((droplet.dir.m_x * droplet.dir.m_x) + (droplet.dir.m_y * droplet.dir.m_y));
            if (length != 0) {
                droplet.dir.m_x /= length;
                droplet.dir.m_y /= length;
            }

            droplet.pos.m_x += droplet.dir.m_x;
            droplet.pos.m_y += droplet.dir.m_y;

            m_dropletTrailPoints.push_back(ngl::Vec4(droplet.pos.m_x, originalTerrainHeight, droplet.pos.m_y, static_cast<float>(droplet.lifetime)));

            droplet.lifetime--;
            if (droplet.lifetime <= 0 || droplet.water <= 0.1f ) {
                break; // End this droplet's simulation
            }
            // If droplet moves off map, also break
            if (droplet.pos.m_x < 0.0f || droplet.pos.m_x >= m_width * m_spacing ||
                droplet.pos.m_y < 0.0f || droplet.pos.m_y >= m_depth * m_spacing) {
                break;
            }


            float newHeight = getHeightAndGradient(droplet.pos.m_x, droplet.pos.m_y).height;
            float deltaHeight = newHeight - originalTerrainHeight;
            //std::cout << "S[" << step << "] NewHeight: " << newHeight << ", DeltaH: " << deltaHeight << std::endl;

            float sedimentCapacity = std::max(-deltaHeight * droplet.speed * droplet.water * m_sedimentCapacityFactor, m_minSedimentCapacity);
            //std::cout << "S[" << step << "] SedimentCapacity: " << sedimentCapacity << " (MinCapacity: " << m_minSedimentCapacity << ")" << std::endl;

            // If carrying more sediment than capacity, deposit sediment
            if (droplet.sediment > sedimentCapacity || deltaHeight > 0)
            {
                //std::cout << "S[" << step << "] *** DEPOSITION TRIGGERED ***" << std::endl;
                //std::cout << "S[" << step << "] DEPO: DropletSediment_Before: " << droplet.sediment << ", SedimentCapacity: " << sedimentCapacity << std::endl;
                float calculated_deposit_amount = 0.0f;
                if (deltaHeight > 0) {
                    // Original:
                    // calculated_deposit_amount = std::min(deltaHeight, droplet.sediment);
                    // Potentially less aggressive:
                    calculated_deposit_amount = std::min(deltaHeight, droplet.sediment) * m_depositionRate; // Or a new, smaller rate
                } else {
                    calculated_deposit_amount = (droplet.sediment - sedimentCapacity) * m_depositionRate;
                }

                float amountToDeposit = std::max(0.0f, calculated_deposit_amount);
                amountToDeposit = std::min(amountToDeposit, droplet.sediment);
                //std::cout << "S[" << step << "] DEPO: CalculatedAmount: " << calculated_deposit_amount << ", FinalAmountToDeposit: " << amountToDeposit << std::endl;

                droplet.sediment -= amountToDeposit;
                float gridFloatX = droplet.pos.m_x / m_spacing;
                float gridFloatZ = droplet.pos.m_y / m_spacing; // Assuming droplet.pos.m_y is world Z

                int nodeX = static_cast<int>(gridFloatX);
                int nodeZ = static_cast<int>(gridFloatZ);

                float cellOffsetX = gridFloatX - nodeX;
                float cellOffsetZ = gridFloatZ - nodeZ;
                //std::cout << "Deposition: gridFloatX=" << gridFloatX << ", gridFloatZ=" << gridFloatZ
                          //<< ", nodeX=" << nodeX << ", nodeZ=" << nodeZ
                          //<< ", cellOffsetX=" << cellOffsetX << ", cellOffsetZ=" << cellOffsetZ << std::endl;
                //std::cout << "amountToDeposit: " << amountToDeposit << std::endl;

                if (nodeX >= 0 && nodeX < m_width - 1 && nodeZ >= 0 && nodeZ < m_depth - 1)
                {

                    int indexNW = nodeZ * m_width + nodeX;
                    int indexNE = indexNW + 1;
                    int indexSW = indexNW + m_width;
                    int indexSE = indexSW + 1;

                    float depositNW = amountToDeposit * (1 - cellOffsetX) * (1 - cellOffsetZ);
                    float depositNE = amountToDeposit * cellOffsetX * (1 - cellOffsetZ);
                    float depositSW = amountToDeposit * (1 - cellOffsetX) * cellOffsetZ;
                    float depositSE = amountToDeposit * cellOffsetX * cellOffsetZ;

                    m_heightGrid[indexNW].m_y += depositNW;
                    m_heightGrid[indexNE].m_y += depositNE;
                    m_heightGrid[indexSW].m_y += depositSW;
                    m_heightGrid[indexSE].m_y += depositSE;

                }

                //std::cout << "S[" << step << "] DEPO: DropletSediment_After: " << droplet.sediment << std::endl;

            }
            else
            {
                    //std::cout << "S[" << step << "] *** EROSION TRIGGERED/Attempted ***" << std::endl;
                float amountToErode = std::min((sedimentCapacity - droplet.sediment) * m_erosionRate, -deltaHeight);
                    //std::cout << "S[" << step << "] ERO: InitialAmountToErode: " << amountToErode << std::endl;
                    //std::cout << "S[" << step << "] ERO: DropletSediment_BeforeErosionLoop: " << droplet.sediment << std::endl;
                int currentCellGridX = static_cast<int>(droplet.pos.m_x / m_spacing);
                int currentCellGridZ = static_cast<int>(droplet.pos.m_y / m_spacing); // Assuming droplet.pos.m_y is world Z
                // Clamp to valid grid range
                currentCellGridX = std::max(0, std::min(currentCellGridX, (int)m_width - 1));
                currentCellGridZ = std::max(0, std::min(currentCellGridZ, (int)m_depth - 1));
                int brushAccessIndex = currentCellGridZ * m_width + currentCellGridX;

                if (brushAccessIndex >= 0 && brushAccessIndex <= m_brushIndices.size()) { // Check bounds
                    const std::vector<int>& indices = m_brushIndices[brushAccessIndex];
                    const std::vector<float>& weights = m_brushWeights[brushAccessIndex];
                    for (size_t i = 0; i < indices.size(); i++) {

                        int nodeIndex = indices[i];
                        float weight = weights[i];

                        float erosion = amountToErode * weight;
                        float actualErosion = std::min(m_heightGrid[nodeIndex].m_y, erosion);
                        m_heightGrid[nodeIndex].m_y -= actualErosion;
                        droplet.sediment += actualErosion;

                        // std::cout << "Eroded " << actualErosion << " from node at index " << nodeIndex
                        //           << " (X: " << m_heightGrid[nodeIndex].m_x
                        //           << ", Z: " << m_heightGrid[nodeIndex].m_z << ")\n";
                    }
                } else {
                    // Handle case where brushAccessIndex is out of bounds, though clamping should prevent this
                    //std::cout << "Error: Brush access index out of bounds!" << std::endl;
                    continue; // Skip erosion for this step if brush can't be found
                }
            //std::cout << "S[" << step << "] ERO: DropletSediment_AfterErosionLoop: " << droplet.sediment << std::endl; // Print this after the brush loop finishes
            }

        droplet.speed = std::sqrt(std::max(0.0f, droplet.speed * droplet.speed + (-deltaHeight) * m_gravity));
        droplet.water *= (1.0f - m_evaporationRate);
        // //update droplet's speed
        // //std::cout << "S[" << step << "] EndStepSpeed: " << droplet.speed << ", EndStepWater: " << droplet.water << std::endl;
        // //std::cout << "S[" << step << "] --- End of Step ---" << std::endl << std::endl;
        }
    }
    // After the simulation, m_dropletTrailPoints contains all the path points.
    refreshGPUAssets();
    std::cout<<"done"<<std::endl;
}

void Plane::computeAreaOfInfluence(float radius)
{
    int centerX = m_width / 2;
    int centerY = m_depth / 2;

    m_brushIndices.resize(m_width * m_depth);
    m_brushWeights.resize(m_width * m_depth);

    std::vector<int> relativeX;
    std::vector<int> relativeY;
    std::vector<float> unnormalizedWeights;

    float weightSum = 0.0f;

    // Relative circle pattern
    for (int offsetY = -radius; offsetY <= radius; ++offsetY) {
        for (int offsetX = -radius; offsetX <= radius; ++offsetX) {
            float distanceSquared = offsetX * offsetX + offsetY * offsetY;

            if (distanceSquared < radius * radius) {
                float distance = std::sqrt(distanceSquared);
                float weight = 1.0f - (distance / radius); // stronger near center

                relativeX.push_back(offsetX);
                relativeY.push_back(offsetY);
                unnormalizedWeights.push_back(weight);
                weightSum += weight;
            }
        }
    }

    // Normalize weights
    for (float& weight : unnormalizedWeights) {
        weight /= weightSum;
    }

    // Apply that pattern to each cell on the grid
    for (int y = 0; y < m_depth; ++y) {
        for (int x = 0; x < m_width; ++x) {
            int centerIndex = y * m_width + x;

            std::vector<int>& indices = m_brushIndices[centerIndex];
            std::vector<float>& weights = m_brushWeights[centerIndex];
            indices.clear();
            weights.clear();

            for (size_t i = 0; i < relativeX.size(); ++i) {
                int nx = x + relativeX[i];
                int ny = y + relativeY[i];

                if (nx >= 0 && nx < m_width && ny >= 0 && ny < m_depth) {
                    int index = ny * m_width + nx;
                    indices.push_back(index);
                    weights.push_back(unnormalizedWeights[i]);
                }
            }
        }
    }
}

//Sebastian Lague's Erosion outline:
    // Erode(map)
    // {
    //     // Create water droplet at random point on the map
    //     for(lifetime 1; lifetime 30; liftime ++)
    //         // Calculate droplets height and the direction of flow with bilinear interpolation of surrounding heights

    //         // Update the droplets position (move 1 unit regardless of speed so as not to skip over sections of the map)

    //         // Find the droplet's new height and calculate the deltaHeight

    //         // Calculate the droplet's sediment capacity based on its speed and how much water is in the droplet (higher when moving fast down a slope and contains lots of water)

    //         // If calculating more sediment than capacity, or if flowing up a slope (due to its inertia)
    //         // Deposit a fraction of the sediment to the surrounding nodes (with bilinear interpolation)

    //         // Otherwise erode a fraction of the droplets remaining capacity from the soil, distributed over the radius of the droplet.
    //         // Note: don't erode more than deltaHeight to avoid digging holes behind the droplet and creating sikes

    //         // Update droplet's speed based on deltaHeight
    //         // Evaporate a fraction of the droplet's water






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
    m_dropletTrailPoints.clear();
    applyPerlinNoiseToGrid();


    //applyHydraulicErosion(1,30);

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
