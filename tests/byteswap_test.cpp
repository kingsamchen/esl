// Copyright (c) 2023 Kingsley Chen <kingsamchen at gmail dot com>
// This file is subject to the terms of license that can be found
// in the LICENSE file.

#include <cstdint>

#include "doctest/doctest.h"

#include "esl/byteswap.h"

namespace {

TEST_SUITE_BEGIN("byteswap");

TEST_CASE("reverse bytes order for a 16-bit integer") {
    SUBCASE("for unsigned") {
        const std::uint16_t n{0x1234}; // NOLINT(readability-magic-numbers)
        CHECK_EQ(esl::byteswap(n), std::uint16_t{0x3412});
    }

    SUBCASE("for signed") {
        const std::int16_t n{0x77ff}; // NOLINT(readability-magic-numbers)
        CHECK_EQ(esl::byteswap(n), static_cast<std::int16_t>(std::uint16_t{0xff77}));
    }
}

TEST_CASE("reverse bytes order for a 32-bit integer") {
    SUBCASE("for unsigned") {
        const std::uint32_t n{0xdeadbeef}; // NOLINT(readability-magic-numbers)
        CHECK_EQ(esl::byteswap(n), std::uint32_t{0xefbeadde});
    }

    SUBCASE("for signed") {
        const std::int32_t n{0x01234567}; // NOLINT(readability-magic-numbers)
        CHECK_EQ(esl::byteswap(n), static_cast<std::int32_t>(std::uint32_t{0x67452301}));
    }
}

TEST_CASE("reverse bytes order for a 64-bit integer") {
    SUBCASE("for unsigned") {
        const std::uint64_t n{0xdeadbeefbaddcafe}; // NOLINT(readability-magic-numbers)
        CHECK_EQ(esl::byteswap(n), std::uint64_t{0xfecaddbaefbeadde});
    }

    SUBCASE("for signed") {
        const std::int64_t n{0x0123456789abcdef}; // NOLINT(readability-magic-numbers)
        CHECK_EQ(esl::byteswap(n), static_cast<std::int64_t>(std::uint64_t{0xefcdab8967452301}));
    }
}

TEST_SUITE_END();

} // namespace
