#pragma once

#include <cstdint>
#include <memory>

namespace Rte {

    using i8 = int8_t;
    using i16 = int16_t;
    using i32 = int32_t;
    using i64 = int64_t;

    using u8 = uint8_t;
    using u16 = uint16_t;
    using u32 = uint32_t;
    using u64 = uint64_t;

    using f32 = float;
    using f64 = double;

    template<typename T>
    struct Vec2 {
        T x;
        T y;
    };

    template<typename T>
    struct Vec3 {
        T x;
        T y;
        T z;
    };

    template<typename T>
    struct Vec4 {
        T x;
        T y;
        T z;
        T w;
    };


    /**
     * @brief Casts a shared pointer of type void to a shared pointer of type T.
     *
     * @tparam T Type to cast to.
     * @param ptr Shared pointer of type void.
     * @return std::shared_ptr<T> Shared pointer of type T.
     */
    template<typename T>
    std::shared_ptr<T> interfaceCast(std::shared_ptr<void> ptr) {
        return std::reinterpret_pointer_cast<T>(ptr);
    }

}   // namespace Rte

#ifdef _WIN32
    #define EXPORT __declspec(dllexport)
#else
    #define EXPORT __attribute__((visibility("default")))
#endif
