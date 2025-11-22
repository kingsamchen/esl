// Copyright (c) 2025 Kingsley Chen <kingsamchen at gmail dot com>
// This file is subject to the terms of license that can be found
// in the LICENSE file.

#include <cstdint>
#include <string>
#include <variant>

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

TEST_CASE("use overloaded for variant visit") {
    std::variant<int, std::string> vars;

    auto ov = esl::overloaded{
        [](int) -> int {
            return 0;
        },
        [](const std::string&) -> int {
            return 1;
        }};

    SUBCASE("value is int") {
        vars = 42;
        const auto idx = std::visit(ov, vars);
        CHECK_EQ(idx, 0);
    }

    SUBCASE("value is std::string") {
        vars = "hello";
        const auto idx = std::visit(ov, vars);
        CHECK_EQ(idx, 1);
    }
}

TEST_SUITE_END();

} // namespace
