#pragma once

#include <cstdint>

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

}   // namespace Rte

#ifdef _WIN32
    #define EXPORT __declspec(dllexport)
#else
    #define EXPORT __attribute__((visibility("default")))
#endif