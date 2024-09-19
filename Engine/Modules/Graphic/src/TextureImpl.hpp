#pragma once

#include "Rte/Common.hpp"
#include "Rte/Graphic/Texture.hpp"

#include "SFML/Graphics/Texture.hpp"

#include <string>

namespace Rte::Graphic {

    class TextureImpl : public Texture {
        public:
            TextureImpl() = default;
            ~TextureImpl() override = default;

            TextureImpl(const TextureImpl&) = delete;
            TextureImpl& operator=(const TextureImpl&) = delete;

            TextureImpl(TextureImpl&&) noexcept = default;
            TextureImpl& operator=(TextureImpl&&) noexcept = default;

            void create(const Vec2<u16>& size) override;

            void loadFromFile(const std::string& filename) override;
            void loadFromMemory(const u8* data, const Vec2<u16>& size) override;
            void update(const u8* data) override;

            void setSmooth(bool smooth) override;
            void generateMipmap() override;

            [[nodiscard]] Vec2<u16> getSize() const override;
            [[nodiscard]] const u8* getPixels() const override;
            [[nodiscard]] bool isSmooth() const override;


        private:
            sf::Texture m_texture;
    };

}   // namespace Rte::Graphic
