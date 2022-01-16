// Copyright (c) 2022 Kingsley Chen <kingsamchen at gmail dot com>
// This file is subject to the terms of license that can be found
// in the LICENSE file.

#include <string>
#include <string_view>

#include "doctest/doctest.h"

#include "esl/strings.h"

namespace strings = esl::strings;

namespace {

TEST_SUITE_BEGIN("strings/match");

TEST_CASE("starts with") {
    using namespace std::string_literals;
    using namespace std::string_view_literals;

    SUBCASE("normal cases") {
        std::string_view sv = "foobar";
        CHECK(strings::starts_with(sv, sv));
        CHECK(strings::starts_with(sv, "foo"));
        CHECK(strings::starts_with(sv, ""sv));
        CHECK_FALSE(strings::starts_with(sv, "foobaz"));

        std::string_view esv;
        CHECK(strings::starts_with(esv, ""));
        CHECK_FALSE(strings::starts_with(esv, sv));

        std::string str = "123\0abc"s;
        CHECK(strings::starts_with(str, str));
        CHECK(strings::starts_with(str, "123\0"s));
        CHECK(strings::starts_with(str, ""sv));
        CHECK_FALSE(strings::starts_with(str, "abc"));
    }

    SUBCASE("when |str| < |prefix|") {
        std::string str = "foo";
        CHECK_FALSE(strings::starts_with(str, "foobar"));
    }

    SUBCASE("compile time support") {
        static constexpr std::string_view text = "foobar";
        static constexpr std::string_view pref = "foo";
        static_assert(strings::starts_with(text, pref));
    }
}

TEST_CASE("ends with") {
    using namespace std::string_literals;
    using namespace std::string_view_literals;

    SUBCASE("normal cases") {
        std::string_view sv = "foobar";
        CHECK(strings::ends_with(sv, sv));
        CHECK(strings::ends_with(sv, "bar"));
        CHECK(strings::ends_with(sv, ""sv));
        CHECK_FALSE(strings::ends_with(sv, "foobaz"));

        std::string_view esv;
        CHECK(strings::ends_with(esv, ""));
        CHECK_FALSE(strings::ends_with(esv, sv));

        std::string str = "123\0abc"s;
        CHECK(strings::ends_with(str, str));
        CHECK(strings::ends_with(str, "\0abc"s));
        CHECK(strings::ends_with(str, ""sv));
        CHECK_FALSE(strings::ends_with(str, "0abc"));
    }

    SUBCASE("when |str| < |suffix|") {
        std::string str = "bar";
        CHECK_FALSE(strings::ends_with(str, "foobar"));
    }

    SUBCASE("compile time support") {
        static constexpr std::string_view text = "foobar";
        static constexpr std::string_view pref = "bar";
        static_assert(strings::ends_with(text, pref));
    }
}

TEST_SUITE_END();

} // namespace
