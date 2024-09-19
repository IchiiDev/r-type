#pragma once

#include "Rte/Common.hpp"
#include "Rte/Graphic/Texture.hpp"

#include "SFML/Graphics/Texture.hpp"

#include <string>
#include <vector>

namespace Rte::Graphic {

    class TextureImpl : public Texture {
        public:
            TextureImpl() = default;
            ~TextureImpl() override = default;

            TextureImpl(const TextureImpl&) = delete;
            TextureImpl& operator=(const TextureImpl&) = delete;

            TextureImpl(TextureImpl&&) noexcept = default;
            TextureImpl& operator=(TextureImpl&&) noexcept = default;


            // Overriden methods
            void loadFromFile(const std::string& filename) override;
            void loadFromMemory(const u8* data, const Vec2<u16>& size) override;

            void setSmooth(bool smooth) override;
            void generateMipmap() override;

            [[nodiscard]] Vec2<u16> getSize() const override;
            [[nodiscard]] std::vector<u8> getPixels() const override;
            [[nodiscard]] bool isSmooth() const override;


            // Non-virtual functions
            [[nodiscard]] const sf::Texture& getHandle() const;


        private:
            sf::Texture m_texture;
    };

}   // namespace Rte::Graphic
