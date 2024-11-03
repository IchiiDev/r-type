#include "Rte/BasicComponents.hpp"
#include "Rte/Common.hpp"
#include "Rte/Ecs/Types.hpp"
#include "ServerApp.hpp"
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <numbers>
#include <vector>

#include "Rte/Physics/Materials.hpp"

// Perlin noise function for 1D with octaves
float randomCurve(float x, int octaves = 1, float persistence = 0.5) {
    return ((sin (2 * x) + sin(std::numbers::pi * x) + (sin (2 * (x + 1)) + sin(std::numbers::pi * 0.3 * (x + 1))))) / 2;
}

Rte::Entity ServerApp::generateTerrainBlock(int width, int maxHeight, Rte::Vec2<float> pos) {
    std::vector<Rte::u8> terrainBlock(static_cast<size_t>((width + 2) * (maxHeight + 2)) * 4);
    for (int i = 0; i < terrainBlock.size(); i++) {
        terrainBlock[i] = 0;
    }

    for (int i = 2; i < width; i++) {
        // Generate a Perlin noise value for the given x coordinate
        float noise = randomCurve(terrainPos);

        // Scale the noise value to the desired range
        int terrainHeight = noise * (maxHeight / 10) + static_cast<float>(maxHeight) / 2;
        terrainHeight = std::clamp(terrainHeight, 0, maxHeight - 1);

        for (int j = 2; j < maxHeight; j++) {
            // Set the color of the pixel based on the terrain height
            if (j >= terrainHeight) {
                terrainBlock[static_cast<size_t>(j * width + i) * 4] = Rte::Physics::invMatColors.at(Rte::Physics::MaterialType::STATIC_WOOD).r;
                terrainBlock[static_cast<size_t>(j * width + i) * 4 + 1] = Rte::Physics::invMatColors.at(Rte::Physics::MaterialType::STATIC_WOOD).g;
                terrainBlock[static_cast<size_t>(j * width + i) * 4 + 2] = Rte::Physics::invMatColors.at(Rte::Physics::MaterialType::STATIC_WOOD).b;
                terrainBlock[static_cast<size_t>(j * width + i) * 4 + 3] = Rte::Physics::invMatColors.at(Rte::Physics::MaterialType::STATIC_WOOD).a;
            } else {
                terrainBlock[static_cast<size_t>(j * width + i) * 4] = 0;
                terrainBlock[static_cast<size_t>(j * width + i) * 4 + 1] = 0;
                terrainBlock[static_cast<size_t>(j * width + i) * 4 + 2] = 0;
                terrainBlock[static_cast<size_t>(j * width + i) * 4 + 3] = 0;
            }
        }
        terrainPos += 0.03;
    }
    return createBreakable(pos, terrainBlock, terrainBlock, {static_cast<Rte::u16>(width), static_cast<Rte::u16>(maxHeight)});
}

void ServerApp::updateTerrain() {
    if (m_ecs->getComponent<Rte::BasicComponents::Transform>(m_terrains.at(0)).position.x < -1920/2 - m_terrainBlockWidth) {
        destroyBreakable(m_terrains.at(0));
        m_terrains.erase(m_terrains.begin());
        m_terrains.push_back(generateTerrainBlock(
                m_terrainBlockWidth,
                m_terrainBlockHeight,
                {
                    m_ecs->getComponent<Rte::BasicComponents::Transform>(m_terrains.at(m_terrains.size() - 1)).position.x + m_terrainBlockWidth * 8 - (1920 / 2) - 16,
                    static_cast<float>(1080 / 2 - m_terrainBlockHeight / 2 * 8)
                }
        ));
    }
}