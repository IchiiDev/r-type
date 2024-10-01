#include "Rte/Common.hpp"

#include "Rte/Physics/Materials.hpp"
#include "SandBoxImpl.hpp"

#include <cstdlib>

using namespace Rte::Physics;

SandBoxImpl::SandBoxImpl(Vec2<u16> size) : m_size(size) {
    m_canvas.resize(static_cast<size_t>(size.x * size.y));
    for (int i = 0; i < size.x * size.y; i++)
        m_canvas.at(i) = {MaterialType::AIR, {0, 0, 0, 0}, 0};
}

const std::vector<Pixel>& SandBoxImpl::getCanvas() {
    return m_canvas;
}

const Rte::Vec2<Rte::u16>& SandBoxImpl::getSize() {
    return m_size;
}

void SandBoxImpl::changePixel(const Vec2<int>& pos, const Pixel& pixel) {
    if (pos.x < 0 || pos.x >= m_size.x || pos.y < 0 || pos.y >= m_size.y)
        return;

    m_canvas.at(pos.y * m_size.x + pos.x) = pixel;
}

const std::vector<Particle>& SandBoxImpl::getParticles() {
    return m_particles;
}

void SandBoxImpl::addParticle(const Particle& particle) {
    m_particles.push_back(particle);
}

void SandBoxImpl::Step() {  // NOLINT (readability-function-cognitive-complexity)
    std::vector<Vec2<int>> pixelsToChange;
    for (int y = 0; y < m_size.y; y++) {
        for (int x = 0; x < m_size.x; x++) {
            if (x > 0 && x < m_size.x - 1 && y > 0 && y < m_size.y - 1) {
                switch (mats.at(m_canvas.at(y * m_size.x + x).mat).comp) {
                    case Comportement::POWDER:
                        if (mats.at(m_canvas.at((y + 1) * m_size.x + x).mat).comp == Comportement::NOTHING || mats.at(m_canvas.at((y + 1) * m_size.x + x).mat).comp     == Comportement::LIQUID ||
                        mats.at(m_canvas.at((y + 1) * m_size.x + x - 1).mat).comp == Comportement::NOTHING || mats.at(m_canvas.at((y + 1) * m_size.x + x - 1).mat).comp == Comportement::LIQUID ||
                        mats.at(m_canvas.at((y + 1) * m_size.x + x + 1).mat).comp == Comportement::NOTHING || mats.at(m_canvas.at((y + 1) * m_size.x + x + 1).mat).comp == Comportement::LIQUID )
                            pixelsToChange.push_back({x, y});
                        break;

                    case Comportement::LIQUID:
                        if (mats.at(m_canvas.at((y + 1) * m_size.x + x).mat).comp == Comportement::NOTHING ||
                        mats.at(m_canvas.at((y + 1) * m_size.x + x - 1).mat).comp == Comportement::NOTHING ||
                        mats.at(m_canvas.at((y + 1) * m_size.x + x + 1).mat).comp == Comportement::NOTHING ||
                        mats.at(m_canvas.at(y * m_size.x + x - 1).mat).comp == Comportement::NOTHING ||
                        mats.at(m_canvas.at(y * m_size.x + x + 1).mat).comp == Comportement::NOTHING ||
                        (m_canvas.at((y) * m_size.x + x).mat == MaterialType::ACID && mats.at(m_canvas.at((y + 1) * m_size.x + x).mat).comp == Comportement::POWDER))
                            pixelsToChange.push_back({x, y});
                        break;

                    default:
                        break;
                    }
            }
        }
    }


    Pixel tempPixel{};
    for (const Vec2<int>& pos : pixelsToChange) {
        switch (mats.at(m_canvas.at(pos.y * m_size.x + pos.x).mat).comp) {
            case Comportement::LIQUID:
                if (m_canvas.at((pos.y + 1) * m_size.x + pos.x).mat == MaterialType::AIR) {
                    tempPixel = m_canvas.at((pos.y + 1) * m_size.x + pos.x);
                    m_canvas.at((pos.y + 1) * m_size.x + pos.x) = m_canvas.at(pos.y * m_size.x + pos.x);
                    m_canvas.at(pos.y * m_size.x + pos.x) = tempPixel;
                    break;
                }
                if (rand() % 10 == 1) {
                    if (m_canvas.at((pos.y) * m_size.x + pos.x).mat == MaterialType::ACID) {
                        if (mats.at(m_canvas.at((pos.y + 1) * m_size.x + pos.x).mat).comp == Comportement::POWDER) {
                            if (rand() % 2 == 1) {
                                m_canvas.at((pos.y + 1) * m_size.x + pos.x) = {MaterialType::AIR, {0, 0, 0, 0}, 0};
                            } else {
                                m_canvas.at((pos.y + 1) * m_size.x + pos.x) = m_canvas.at(pos.y * m_size.x + pos.x);
                            }
                            m_canvas.at(pos.y * m_size.x + pos.x) = {MaterialType::AIR, {0, 0, 0, 0}, 0};
                            break;
                        }
                    }
                }
                if (rand() % 2 == 0) {
                    for (int offset = pos.x; offset < m_size.x; offset++) {
                        if (mats.at(m_canvas.at((pos.y) * m_size.x + offset + 1).mat).comp != Comportement::NOTHING) {
                            break;
                        }
                        if (mats.at(m_canvas.at((pos.y + 1) * m_size.x + offset).mat).comp == Comportement::NOTHING) {
                            tempPixel = m_canvas.at((pos.y + 1) * m_size.x + offset);
                            m_canvas.at((pos.y + 1) * m_size.x + offset) = m_canvas.at(pos.y * m_size.x + pos.x);
                            m_canvas.at(pos.y * m_size.x + pos.x) = tempPixel;
                            break;
                        }
                    }
                } else {
                    for (int offset = pos.x; offset > 0; offset--) {
                        if (mats.at(m_canvas.at((pos.y) * m_size.x + offset - 1).mat).comp != Comportement::NOTHING) {
                            break;
                        }
                        if (mats.at(m_canvas.at((pos.y + 1) * m_size.x + offset).mat).comp == Comportement::NOTHING) {
                            tempPixel = m_canvas.at((pos.y + 1) * m_size.x + offset);
                            m_canvas.at((pos.y + 1) * m_size.x + offset) = m_canvas.at(pos.y * m_size.x + pos.x);
                            m_canvas.at(pos.y * m_size.x + pos.x) = tempPixel;
                            break;
                        }
                    }
                }
                break;

            case Comportement::POWDER:
                if (mats.at(m_canvas.at((pos.y + 1) * m_size.x + pos.x).mat).comp == Comportement::NOTHING) {
                    tempPixel = m_canvas.at((pos.y + 1) * m_size.x + pos.x);
                    m_canvas.at((pos.y + 1) * m_size.x + pos.x) = m_canvas.at(pos.y * m_size.x + pos.x);
                    m_canvas.at(pos.y * m_size.x + pos.x) = tempPixel;
                    break;
                }
                if (rand() % 2 == 0) {
                    if (mats.at(m_canvas.at((pos.y + 1) * m_size.x + pos.x - 1).mat).comp == Comportement::NOTHING
                    ||  mats.at(m_canvas.at((pos.y + 1) * m_size.x + pos.x - 1).mat).comp == Comportement::LIQUID) {
                        tempPixel = m_canvas.at((pos.y + 1) * m_size.x + pos.x - 1);
                        m_canvas.at((pos.y + 1) * m_size.x + pos.x - 1) = m_canvas.at(pos.y * m_size.x + pos.x);
                        m_canvas.at(pos.y * m_size.x + pos.x) = tempPixel;
                        break;
                    }
                } else {
                    if (mats.at(m_canvas.at((pos.y + 1) * m_size.x + pos.x + 1).mat).comp == Comportement::NOTHING
                    ||  mats.at(m_canvas.at((pos.y + 1) * m_size.x + pos.x + 1).mat).comp == Comportement::LIQUID) {
                        tempPixel = m_canvas.at((pos.y + 1) * m_size.x + pos.x + 1);
                        m_canvas.at((pos.y + 1) * m_size.x + pos.x + 1) = m_canvas.at(pos.y * m_size.x + pos.x);
                        m_canvas.at(pos.y * m_size.x + pos.x) = tempPixel;
                        break;
                    }
                }
                if (mats.at(m_canvas.at((pos.y + 1) * m_size.x + pos.x).mat).comp == Comportement::LIQUID) {
                    tempPixel = m_canvas.at((pos.y + 1) * m_size.x + pos.x);
                    m_canvas.at((pos.y + 1) * m_size.x + pos.x) = m_canvas.at(pos.y * m_size.x + pos.x);
                    m_canvas.at(pos.y * m_size.x + pos.x) = tempPixel;
                    break;
                }
                break;

            default:
                break;
        }
    }

    for (int i = 0; i < m_particles.size(); i++) {
        if (mats.at(m_canvas.at(m_particles.at(i).pos.y * m_size.x + m_particles.at(i).pos.x).mat).comp == Comportement::NOTHING) {
            m_canvas.at(m_particles.at(i).pos.y * m_size.x + m_particles.at(i).pos.x) = m_particles.at(i).pixel;
            m_particles.erase(m_particles.begin() + i);
        }
    }
}
