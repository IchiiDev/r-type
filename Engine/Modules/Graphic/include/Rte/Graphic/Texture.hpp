#pragma once

#include "Rte/Common.hpp"

#include <string>
#include <vector>

namespace Rte::Graphic {

    class Texture {
        public:
            Texture() = default;
            virtual ~Texture() = default;

            Texture(const Texture&) = delete;
            Texture& operator=(const Texture&) = delete;
            Texture(Texture&&) noexcept = default;
            Texture& operator=(Texture&&) noexcept = default;

            virtual void create(const Vec2<u16>& size) = 0;

            virtual void loadFromFile(const std::string& filename) = 0;
            virtual void loadFromMemory(const u8* data, const Vec2<u16>& size) = 0;
            virtual void update(const u8* data) = 0;

            virtual void setSmooth(bool smooth) = 0;
            virtual void generateMipmap() = 0;

            [[nodiscard]] virtual Vec2<u16> getSize() const = 0;
            [[nodiscard]] virtual std::vector<u8> getPixels() const = 0;
            [[nodiscard]] virtual bool isSmooth() const = 0;
    };

}   // namespace Rte::Graphic
