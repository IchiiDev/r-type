#pragma once

#include "Rte/Common.hpp"

#include <string>

namespace Rte::Graphic {

    class Texture {
        public:
            Texture() = default;
            virtual ~Texture() = default;

            Texture(const Texture&) = delete;
            Texture& operator=(const Texture&) = delete;
            Texture(Texture&&) noexcept = default;
            Texture& operator=(Texture&&) noexcept = default;

            /**
             * @brief Load the texture from a file (bmp, png, tga, jpg, gif, psd, hdr, pic or pnm).
             *
             * @param filename Path to the file.
             */
            virtual void loadFromFile(const std::string& filename) = 0;

            /**
             * @brief Load the texture from an array of pixels.
             *
             * @param data Array of pixels.
             * @param size Size of the texture.
             */
            virtual void loadFromMemory(const u8* data, const Vec2<u16>& size) = 0;


            /**
             * @brief Set the smooth property of the texture.
             *
             * @param smooth True to enable smoothing, false to disable it.
             */
            virtual void setSmooth(bool smooth) = 0;

            /**
             * @brief Generate the mipmap of the texture.
             */
            virtual void generateMipmap() = 0;


            /**
             * @brief Get the size of the texture.
             *
             * @return Vec2<u16> Size of the texture.
             */
            [[nodiscard]] virtual Vec2<u16> getSize() const = 0;

            /**
             * @brief Get the pixels of the texture.
             *
             * @return u8* Pixels of the texture.
             */
            [[nodiscard]] virtual const u8 *getPixels() const = 0;

            /**
             * @brief Check if the texture is smooth.
             *
             * @return bool True if the texture is smooth, false otherwise.
             */
            [[nodiscard]] virtual bool isSmooth() const = 0;
    };

}   // namespace Rte::Graphic
