// Copyright (c) 2025 Kingsley Chen <kingsamchen at gmail dot com>
// This file is subject to the terms of license that can be found
// in the LICENSE file.

#include <cstdint>

#include "doctest/doctest.h"

#include "esl/utility.h"

namespace {

enum class Flags : std::uint8_t {
    normal = 0x0,
    special = 0x1,
    internal = 0x1 << 1,
};

TEST_SUITE_BEGIN("utility");

TEST_CASE("use to_underlying to convert an enum member to its underlying value") {
    const Flags flags{Flags::internal};
    auto val = esl::to_underlying(flags);
    static_assert(std::is_same_v<decltype(val), std::uint8_t>);
    CHECK_EQ(val, (0x1 << 1));
}

TEST_SUITE_END();

} // namespace
