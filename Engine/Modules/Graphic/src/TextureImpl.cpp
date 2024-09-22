#include "Rte/Common.hpp"
#include "TextureImpl.hpp"

#include "SFML/Graphics/Image.hpp"
#include "SFML/Graphics/Texture.hpp"

#include <stdexcept>
#include <string>

using namespace Rte::Graphic;

void TextureImpl::loadFromFile(const std::string& filename) {
    if (!m_texture.loadFromFile(filename))
        throw std::runtime_error("Failed to load texture from file");
    m_image = m_texture.copyToImage();
}

void TextureImpl::loadFromMemory(const u8* data, const Vec2<u16>& size) {
    if (!m_texture.resize({size.x, size.y}))
        throw std::runtime_error("Failed to create texture");

    m_texture.update(data);
    m_image = m_texture.copyToImage();
}

void TextureImpl::setSmooth(bool smooth) {
    m_texture.setSmooth(smooth);
}

Rte::Vec2<Rte::u16> TextureImpl::getSize() const {
    const auto size = m_texture.getSize();
    return {static_cast<u16>(size.x), static_cast<u16>(size.y)};
}

const Rte::u8 *TextureImpl::getPixels() const {
    return m_image.getPixelsPtr();
}

bool TextureImpl::isSmooth() const {
    return m_texture.isSmooth();
}

void TextureImpl::generateMipmap() {
    if (!m_texture.generateMipmap())
        throw std::runtime_error("Failed to generate mipmap");
}

const sf::Texture& TextureImpl::getHandle() const {
    return m_texture;
}
