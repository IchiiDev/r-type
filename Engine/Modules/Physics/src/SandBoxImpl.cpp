#include "Rte/Common.hpp"

#include "SandBoxImpl.hpp"
#include "Rte/Physics/Materials.hpp"

#include <cstddef>
#include <cstdlib>

using namespace Rte::Physics;

//Where image is an array of pixels in rgba format
SandBoxImpl::SandBoxImpl(Vec2<u16> size) : m_size(size) {
    m_canvas.resize(size.x * size.y);
    for (int i = 0; i < size.x * size.y; i++) {
        m_canvas[i] = air;
    }
}

std::vector<materials_t> SandBoxImpl::getCanvas() const {
    return m_canvas;
}

void SandBoxImpl::changeMaterial(Vec2<int> pos, materials_t material) {
    if (pos.x < 0 || pos.x >= m_size.x || pos.y < 0 || pos.y >= m_size.y) {
        return;
    }
    m_canvas[pos.y * m_size.x + pos.x] = material;
}

void SandBoxImpl::Step() {
    std::vector<Vec2<int>> pixelsToChange;
    for (int y = 0; y < m_size.y; y++) {
        for (int x = 0; x < m_size.x; x++) {
            if (x > 0 && x < m_size.x - 1 && y > 0 && y < m_size.y - 1) {
                switch (m_canvas[y * m_size.x + x]) {
                case materials_t::sand:
                    if (m_canvas[(y + 1) * m_size.x + x] == materials_t::air || m_canvas[(y + 1) * m_size.x + x] == materials_t::water ||
                    m_canvas[(y + 1) * m_size.x + x - 1] == materials_t::air || m_canvas[(y + 1) * m_size.x + x - 1] == materials_t::water ||
                    m_canvas[(y + 1) * m_size.x + x + 1] == materials_t::air || m_canvas[(y + 1) * m_size.x + x + 1] == materials_t::water )
                        pixelsToChange.push_back({x, y});
                case materials_t::water:
                    if (m_canvas[(y + 1) * m_size.x + x] == materials_t::air ||
                    m_canvas[(y + 1) * m_size.x + x - 1] == materials_t::air ||
                    m_canvas[(y + 1) * m_size.x + x + 1] == materials_t::air ||
                    m_canvas[y * m_size.x + x - 1] == materials_t::air ||
                    m_canvas[y * m_size.x + x + 1] == materials_t::air)
                        pixelsToChange.push_back({x, y});
                }
            }
        }
    }
    for (const Vec2<int>& pos : pixelsToChange) {
        switch (m_canvas[pos.y * m_size.x + pos.x]) {
            case materials_t::water:
                if (m_canvas[(pos.y + 1) * m_size.x + pos.x] == materials_t::air) {
                    m_canvas[(pos.y + 1) * m_size.x + pos.x] = materials_t::water;
                    m_canvas[pos.y * m_size.x + pos.x] = materials_t::air;
                    break;
                }
                if (rand() % 2 == 0) {
                    for (int offset = pos.x; offset < m_size.x; offset++) {
                        if (m_canvas[(pos.y) * m_size.x + offset + 1] != materials_t::air) {
                            m_canvas[pos.y * m_size.x + pos.x] = materials_t::water;
                            break;
                        }
                        if (m_canvas[(pos.y + 1) * m_size.x + offset] == materials_t::air) {
                            m_canvas[(pos.y + 1) * m_size.x + offset] = materials_t::water;
                            m_canvas[pos.y * m_size.x + pos.x] = materials_t::air;
                            break;
                        }
                    }
                } else {
                    for (int offset = pos.x; offset > 0; offset--) {
                        if (m_canvas[(pos.y) * m_size.x + offset - 1] != materials_t::air) {
                            m_canvas[pos.y * m_size.x + pos.x] = materials_t::water;
                            break;
                        }
                        if (m_canvas[(pos.y + 1) * m_size.x + offset] == materials_t::air) {
                            m_canvas[(pos.y + 1) * m_size.x + offset] = materials_t::water;
                            m_canvas[pos.y * m_size.x + pos.x] = materials_t::air;
                            break;
                        }
                    }
                }
                break;
            case materials_t::sand:
                if (m_canvas[(pos.y + 1) * m_size.x + pos.x] == materials_t::air) {
                    m_canvas[(pos.y + 1) * m_size.x + pos.x] = materials_t::sand;
                    m_canvas[pos.y * m_size.x + pos.x] = materials_t::air;
                } else if (m_canvas[(pos.y + 1) * m_size.x + pos.x - 1] == materials_t::air) {
                    m_canvas[(pos.y + 1) * m_size.x + pos.x - 1] = materials_t::sand;
                    m_canvas[pos.y * m_size.x + pos.x] = materials_t::air;
                } else if (m_canvas[(pos.y + 1) * m_size.x + pos.x + 1] == materials_t::air) {
                    m_canvas[(pos.y + 1) * m_size.x + pos.x + 1] = materials_t::sand;
                    m_canvas[pos.y * m_size.x + pos.x] = materials_t::air;
                } else if (m_canvas[(pos.y + 1) * m_size.x + pos.x] == materials_t::water) {
                    m_canvas[(pos.y + 1) * m_size.x + pos.x] = materials_t::sand;
                    m_canvas[pos.y * m_size.x + pos.x] = materials_t::water;
                } else if (m_canvas[(pos.y + 1) * m_size.x + pos.x - 1] == materials_t::water) {
                    m_canvas[(pos.y + 1) * m_size.x + pos.x - 1] = materials_t::sand;
                    m_canvas[pos.y * m_size.x + pos.x] = materials_t::water;
                } else if (m_canvas[(pos.y + 1) * m_size.x + pos.x + 1] == materials_t::water) {
                    m_canvas[(pos.y + 1) * m_size.x + pos.x + 1] = materials_t::sand;
                    m_canvas[pos.y * m_size.x + pos.x] = materials_t::water;
                }
                break;
        }

    }
}

SandBoxImpl::~SandBoxImpl() {
}
