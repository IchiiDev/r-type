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
        m_canvas[i] = {materials_t::air, {0, 0, 0, 0}, 0};
    }
}

std::vector<pixel_t> SandBoxImpl::getCanvas() const {
    return m_canvas;
}

void SandBoxImpl::changePixel(Vec2<int> pos, pixel_t pixel) {
    if (pos.x < 0 || pos.x >= m_size.x || pos.y < 0 || pos.y >= m_size.y) {
        return;
    }
    m_canvas[pos.y * m_size.x + pos.x] = pixel;
}

void SandBoxImpl::Step() {
    std::vector<Vec2<int>> pixelsToChange;
    for (int y = 0; y < m_size.y; y++) {
        for (int x = 0; x < m_size.x; x++) {
            if (x > 0 && x < m_size.x - 1 && y > 0 && y < m_size.y - 1) {
                switch (mats.at(m_canvas[y * m_size.x + x].mat).comp) {
                    case powder:
                        if (mats.at(m_canvas[(y + 1) * m_size.x + x].mat).comp == nothing || mats.at(m_canvas[(y + 1) * m_size.x + x].mat).comp == liquid ||
                        mats.at(m_canvas[(y + 1) * m_size.x + x - 1].mat).comp == nothing || mats.at(m_canvas[(y + 1) * m_size.x + x - 1].mat).comp == liquid ||
                        mats.at(m_canvas[(y + 1) * m_size.x + x + 1].mat).comp == nothing || mats.at(m_canvas[(y + 1) * m_size.x + x + 1].mat).comp == liquid )
                            pixelsToChange.push_back({x, y});
                    case liquid:
                        if (mats.at(m_canvas[(y + 1) * m_size.x + x].mat).comp == nothing ||
                        mats.at(m_canvas[(y + 1) * m_size.x + x - 1].mat).comp == nothing ||
                        mats.at(m_canvas[(y + 1) * m_size.x + x + 1].mat).comp == nothing ||
                        mats.at(m_canvas[y * m_size.x + x - 1].mat).comp == nothing ||
                        mats.at(m_canvas[y * m_size.x + x + 1].mat).comp == nothing)
                            pixelsToChange.push_back({x, y});
                }
            }
        }
    }
    pixel_t tempPixel;
    for (const Vec2<int>& pos : pixelsToChange) {
        switch (mats.at(m_canvas[pos.y * m_size.x + pos.x].mat).comp) {
            case liquid:
                if (m_canvas[(pos.y + 1) * m_size.x + pos.x].mat == materials_t::air) {
                    tempPixel = m_canvas[(pos.y + 1) * m_size.x + pos.x];
                    m_canvas[(pos.y + 1) * m_size.x + pos.x] = m_canvas[pos.y * m_size.x + pos.x];
                    m_canvas[pos.y * m_size.x + pos.x] = tempPixel;
                    break;
                }
                if (rand() % 2 == 0) {
                    for (int offset = pos.x; offset < m_size.x; offset++) {
                        if (mats.at(m_canvas[(pos.y) * m_size.x + offset + 1].mat).comp != nothing) {
                            break;
                        }
                        if (mats.at(m_canvas[(pos.y + 1) * m_size.x + offset].mat).comp == nothing) {
                            tempPixel = m_canvas[(pos.y + 1) * m_size.x + offset];
                            m_canvas[(pos.y + 1) * m_size.x + offset] = m_canvas[pos.y * m_size.x + pos.x];
                            m_canvas[pos.y * m_size.x + pos.x] = tempPixel;
                            break;
                        }
                    }
                } else {
                    for (int offset = pos.x; offset > 0; offset--) {
                        if (mats.at(m_canvas[(pos.y) * m_size.x + offset - 1].mat).comp != nothing) {
                            break;
                        }
                        if (mats.at(m_canvas[(pos.y + 1) * m_size.x + offset].mat).comp == nothing) {
                            tempPixel = m_canvas[(pos.y + 1) * m_size.x + offset];
                            m_canvas[(pos.y + 1) * m_size.x + offset] = m_canvas[pos.y * m_size.x + pos.x];
                            m_canvas[pos.y * m_size.x + pos.x] = tempPixel;
                            break;
                        }
                    }
                }
                break;
            case powder:
                if (mats.at(m_canvas[(pos.y + 1) * m_size.x + pos.x].mat).comp == nothing) {
                    tempPixel = m_canvas[(pos.y + 1) * m_size.x + pos.x];
                    m_canvas[(pos.y + 1) * m_size.x + pos.x] = m_canvas[pos.y * m_size.x + pos.x];
                    m_canvas[pos.y * m_size.x + pos.x] = tempPixel;
                    break;
                }
                if (rand() % 2 == 0) {
                    if (mats.at(m_canvas[(pos.y + 1) * m_size.x + pos.x - 1].mat).comp == nothing
                    ||  mats.at(m_canvas[(pos.y + 1) * m_size.x + pos.x - 1].mat).comp == liquid) {
                        tempPixel = m_canvas[(pos.y + 1) * m_size.x + pos.x - 1];
                        m_canvas[(pos.y + 1) * m_size.x + pos.x - 1] = m_canvas[pos.y * m_size.x + pos.x];
                        m_canvas[pos.y * m_size.x + pos.x] = tempPixel;
                        break;
                    }
                } else {
                    if (mats.at(m_canvas[(pos.y + 1) * m_size.x + pos.x + 1].mat).comp == nothing
                    ||  mats.at(m_canvas[(pos.y + 1) * m_size.x + pos.x + 1].mat).comp == liquid) {
                        tempPixel = m_canvas[(pos.y + 1) * m_size.x + pos.x + 1];
                        m_canvas[(pos.y + 1) * m_size.x + pos.x + 1] = m_canvas[pos.y * m_size.x + pos.x];
                        m_canvas[pos.y * m_size.x + pos.x] = tempPixel;
                        break;
                    }
                }
                if (mats.at(m_canvas[(pos.y + 1) * m_size.x + pos.x].mat).comp == liquid) {
                    tempPixel = m_canvas[(pos.y + 1) * m_size.x + pos.x];
                    m_canvas[(pos.y + 1) * m_size.x + pos.x] = m_canvas[pos.y * m_size.x + pos.x];
                    m_canvas[pos.y * m_size.x + pos.x] = tempPixel;
                    break;
                }
                break;
        }

    }
}

SandBoxImpl::~SandBoxImpl() {
}
