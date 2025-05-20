#include "HydraulicErosion.h"
#include <ngl/Random.h>
#include <algorithm>
#include <cmath>

HydraulicErosion::HydraulicErosion() {
    // Initialize any necessary state
}

void HydraulicErosion::erode(std::vector<ngl::Vec3>& heightGrid,
                            unsigned int width,
                            unsigned int depth,
                            float spacing,
                            int numDroplets,
                            int dropletMaxLifetime)
{
    // Implementation of the erosion algorithm
    // This would be moved from Plane::applyHydraulicErosion

    if (heightGrid.empty()) { return; }

    computeAreaOfInfluence(width, depth, m_erosionRadius);
    m_dropletTrailPoints.clear();

        for (int i = 0; i < numDroplets; ++i)
        {
            // Initialize Droplet (as per previous snippets)
            // ... (startX, startZ, create droplet instance)
            int randGridX = ngl::Random::randomPositiveNumber(width - 1);
            int randGridZ = ngl::Random::randomPositiveNumber(depth - 1);

            float startX = static_cast<float>(randGridX) * spacing;
            float startZ = static_cast<float>(randGridZ) * spacing;
            Droplet droplet(ngl::Vec2(startX, startZ), m_initialSpeed, m_initialWaterAmount, dropletMaxLifetime);

            for (int step = 0; step < dropletMaxLifetime; ++step)
            {
                HeightAndGradientData hgDataOld = getHeightAndGradient(heightGrid, width, depth, spacing, droplet.pos.m_x, droplet.pos.m_y);
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
                if (droplet.pos.m_x < 0.0f || droplet.pos.m_x >= width * spacing ||
                    droplet.pos.m_y < 0.0f || droplet.pos.m_y >= depth * spacing) {
                    break;
                    }


                float newHeight = getHeightAndGradient(heightGrid, width, depth, spacing, droplet.pos.m_x, droplet.pos.m_y).height;
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
                    float gridFloatX = droplet.pos.m_x / spacing;
                    float gridFloatZ = droplet.pos.m_y / spacing; // Assuming droplet.pos.m_y is world Z

                    int nodeX = static_cast<int>(gridFloatX);
                    int nodeZ = static_cast<int>(gridFloatZ);

                    float cellOffsetX = gridFloatX - nodeX;
                    float cellOffsetZ = gridFloatZ - nodeZ;
                    //std::cout << "Deposition: gridFloatX=" << gridFloatX << ", gridFloatZ=" << gridFloatZ
                    //<< ", nodeX=" << nodeX << ", nodeZ=" << nodeZ
                    //<< ", cellOffsetX=" << cellOffsetX << ", cellOffsetZ=" << cellOffsetZ << std::endl;
                    //std::cout << "amountToDeposit: " << amountToDeposit << std::endl;

                    if (nodeX >= 0 && nodeX < width - 1 && nodeZ >= 0 && nodeZ < depth - 1)
                    {

                        int indexNW = nodeZ * width + nodeX;
                        int indexNE = indexNW + 1;
                        int indexSW = indexNW + width;
                        int indexSE = indexSW + 1;

                        float depositNW = amountToDeposit * (1 - cellOffsetX) * (1 - cellOffsetZ);
                        float depositNE = amountToDeposit * cellOffsetX * (1 - cellOffsetZ);
                        float depositSW = amountToDeposit * (1 - cellOffsetX) * cellOffsetZ;
                        float depositSE = amountToDeposit * cellOffsetX * cellOffsetZ;

                        heightGrid[indexNW].m_y += depositNW;
                        heightGrid[indexNE].m_y += depositNE;
                        heightGrid[indexSW].m_y += depositSW;
                        heightGrid[indexSE].m_y += depositSE;

                    }

                    //std::cout << "S[" << step << "] DEPO: DropletSediment_After: " << droplet.sediment << std::endl;

                }
                else
                {
                    //std::cout << "S[" << step << "] *** EROSION TRIGGERED/Attempted ***" << std::endl;
                    float amountToErode = std::min((sedimentCapacity - droplet.sediment) * m_erosionRate, -deltaHeight);
                    //std::cout << "S[" << step << "] ERO: InitialAmountToErode: " << amountToErode << std::endl;
                    //std::cout << "S[" << step << "] ERO: DropletSediment_BeforeErosionLoop: " << droplet.sediment << std::endl;
                    int currentCellGridX = static_cast<int>(droplet.pos.m_x / spacing);
                    int currentCellGridZ = static_cast<int>(droplet.pos.m_y / spacing); // Assuming droplet.pos.m_y is world Z
                    // Clamp to valid grid range
                    currentCellGridX = std::max(0, std::min(currentCellGridX, (int)width - 1));
                    currentCellGridZ = std::max(0, std::min(currentCellGridZ, (int)depth - 1));
                    int brushAccessIndex = currentCellGridZ * width + currentCellGridX;

                    if (brushAccessIndex >= 0 && brushAccessIndex <= m_brushIndices.size()) { // Check bounds
                        const std::vector<int>& indices = m_brushIndices[brushAccessIndex];
                        const std::vector<float>& weights = m_brushWeights[brushAccessIndex];
                        for (size_t i = 0; i < indices.size(); i++) {

                            int nodeIndex = indices[i];
                            float weight = weights[i];

                            float erosion = amountToErode * weight;
                            float actualErosion = std::min(heightGrid[nodeIndex].m_y, erosion);
                            heightGrid[nodeIndex].m_y -= actualErosion;
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
}

HeightAndGradientData HydraulicErosion::getHeightAndGradient(
    const std::vector<ngl::Vec3>& heightGrid,
    unsigned int width,
    unsigned int depth,
    float spacing,
    float worldX,
    float worldZ) const {
    // Implementation moved from Plane::getHeightAndGradient
    HeightAndGradientData result;

    float gridFloatZ = worldZ / spacing;
    float gridFloatX = worldX / spacing;

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
    int c_x0 = std::clamp(x0, 0, static_cast<int>(width) - 1);
    int c_z0 = std::clamp(z0, 0, static_cast<int>(depth) - 1);
    int c_x1 = std::clamp(x1, 0, static_cast<int>(width) - 1);
    int c_z1 = std::clamp(z1, 0, static_cast<int>(depth) - 1);

    // Calculate heights of the four nodes of the droplet's cell
    // index = z * m_width + x
    float hNW = heightGrid[c_z0 * width + c_x0].m_y;
    float hNE = heightGrid[c_z0 * width + c_x1].m_y;
    float hSW = heightGrid[c_z1 * width + c_x0].m_y;
    float hSE = heightGrid[c_z1 * width + c_x1].m_y;

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


void HydraulicErosion::computeAreaOfInfluence(unsigned int width, unsigned int depth, float radius) {
    // Implementation moved from Plane::computeAreaOfInfluence
    int centerX = width / 2;
    int centerY = depth / 2;

    m_brushIndices.resize(width * depth);
    m_brushWeights.resize(width * depth);

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
    for (int y = 0; y < depth; ++y) {
        for (int x = 0; x < width; ++x) {
            int centerIndex = y * width + x;

            std::vector<int>& indices = m_brushIndices[centerIndex];
            std::vector<float>& weights = m_brushWeights[centerIndex];
            indices.clear();
            weights.clear();

            for (size_t i = 0; i < relativeX.size(); ++i) {
                int nx = x + relativeX[i];
                int ny = y + relativeY[i];

                if (nx >= 0 && nx < width && ny >= 0 && ny < depth) {
                    int index = ny * width + nx;
                    indices.push_back(index);
                    weights.push_back(unnormalizedWeights[i]);
                }
            }
        }
    }
}
