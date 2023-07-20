// Copyright (c) 2023 Kingsley Chen <kingsamchen at gmail dot com>
// This file is subject to the terms of license that can be found
// in the LICENSE file.

#pragma once

#include <cstdint>

#if defined(_WIN32)
#include <cstdlib>
#else
#include <byteswap.h>
#endif

namespace esl {

#if defined(_WIN32)

inline std::int16_t byteswap(std::int16_t n) noexcept {
    return static_cast<std::int16_t>(_byteswap_ushort(static_cast<std::uint16_t>(n)));
}

inline std::uint16_t byteswap(std::uint16_t n) noexcept {
    return _byteswap_ushort(n);
}

inline std::int32_t byteswap(std::int32_t n) noexcept {
    static_assert(sizeof(std::uint32_t) == sizeof(unsigned long)); // NOLINT(google-runtime-int)
    return static_cast<std::int32_t>(_byteswap_ulong(static_cast<std::uint32_t>(n)));
}

inline std::uint32_t byteswap(std::uint32_t n) noexcept {
    static_assert(sizeof(std::uint32_t) == sizeof(unsigned long)); // NOLINT(google-runtime-int)
    return _byteswap_ulong(n);
}

inline std::int64_t byteswap(std::int64_t n) noexcept {
    return static_cast<std::int64_t>(_byteswap_uint64(static_cast<std::uint64_t>(n)));
}

inline std::uint64_t byteswap(std::uint64_t n) noexcept {
    return _byteswap_uint64(n);
}

#else

inline std::int16_t byteswap(std::int16_t n) noexcept {
    return static_cast<std::int16_t>(bswap_16(static_cast<std::uint16_t>(n)));
}

inline std::uint16_t byteswap(std::uint16_t n) noexcept {
    return bswap_16(n);
}

inline std::int32_t byteswap(std::int32_t n) noexcept {
    return static_cast<std::int32_t>(bswap_32(static_cast<std::uint32_t>(n)));
}

inline std::uint32_t byteswap(std::uint32_t n) noexcept {
    return bswap_32(n);
}

inline std::int64_t byteswap(std::int64_t n) noexcept {
    return static_cast<std::int64_t>(bswap_64(static_cast<std::uint64_t>(n)));
}

inline std::uint64_t byteswap(std::uint64_t n) noexcept {
    return bswap_64(n);
}

#endif

} // namespace esl
