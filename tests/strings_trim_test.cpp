// Copyright (c) 2023 Kingsley Chen <kingsamchen at gmail dot com>
// This file is subject to the terms of license that can be found
// in the LICENSE file.

#include <string>
#include <string_view>

#include "doctest/doctest.h"

#include "esl/strings.h"

namespace strings = esl::strings;

using namespace std::string_view_literals;

namespace {

TEST_SUITE_BEGIN("strings/trim");

TEST_CASE("trim prefix") {
    static_assert(strings::trim_prefix("foobar"sv, "foo"sv) == "bar"sv);
    static_assert(strings::trim_prefix("foobar"sv, "foobar"sv).empty());
    static_assert(strings::trim_prefix("foobar"sv, "bar"sv) == "foobar"sv);
    static_assert(strings::trim_prefix("foobar"sv, ""sv) == "foobar"sv);
}

TEST_CASE("trim prefix in-place") {
    std::string str{"foobar"};

    SUBCASE("matched prefix is removed from the original str") {
        strings::trim_prefix_inplace(str, "foo");
        CHECK_EQ(str, "bar");
        strings::trim_prefix_inplace(str, "bar");
        CHECK_EQ(str, "");
    }

    SUBCASE("no-op when not matched") {
        strings::trim_prefix_inplace(str, "bar");
        CHECK_EQ(str, "foobar");
        strings::trim_prefix_inplace(str, "");
        CHECK_EQ(str, "foobar");
    }
}

TEST_CASE("trim suffix") {
    static_assert(strings::trim_suffix("foobar"sv, "bar"sv) == "foo"sv);
    static_assert(strings::trim_suffix("foobar"sv, "foobar"sv).empty());
    static_assert(strings::trim_suffix("foobar"sv, "foo"sv) == "foobar"sv);
    static_assert(strings::trim_suffix("foobar"sv, ""sv) == "foobar"sv);
}

TEST_CASE("trim suffix in-place") {
    std::string str{"foobar"};

    SUBCASE("matched suffix is removed from the original str") {
        strings::trim_suffix_inplace(str, "bar");
        CHECK_EQ(str, "foo");
        strings::trim_suffix_inplace(str, "foo");
        CHECK_EQ(str, "");
    }

    SUBCASE("no-op when not matched") {
        strings::trim_suffix_inplace(str, "foo");
        CHECK_EQ(str, "foobar");
        strings::trim_suffix_inplace(str, "");
        CHECK_EQ(str, "foobar");
    }
}

TEST_CASE("trim left") {
    static_assert(strings::trim_left("\n\t\rfoobar\r\t\n"sv, "\t\r\n\v"sv) == "foobar\r\t\n"sv);
    static_assert(strings::trim_left("\n\t\r\r\t\n"sv, "\t\r\n\v"sv).empty());
    static_assert(strings::trim_left("foobar\r\n"sv, "\t\r\n\v"sv) == "foobar\r\n"sv);
    static_assert(strings::trim_left(""sv, "\t\r\n\v"sv).empty());
}

TEST_CASE("trim left inplace") {
    SUBCASE("remove matched in left side") {
        std::string str{"\n\t\rfoobar\r\t\n"};
        strings::trim_left_inplace(str, "\t\r\n\v"sv);
        CHECK_EQ(str, "foobar\r\t\n");
    }

    SUBCASE("every character in str matches") {
        std::string str{"\n\t\r\r\t\n"};
        strings::trim_left_inplace(str, "\t\r\n\v"sv);
        CHECK(str.empty());
    }

    SUBCASE("none character in str matches") {
        std::string str{"foobar\r\n"};
        strings::trim_left_inplace(str, "\t\r\n\v"sv);
        CHECK_EQ(str, "foobar\r\n");
    }

    SUBCASE("empty string") {
        std::string str;
        strings::trim_left_inplace(str, "\t\r\n\v");
        CHECK(str.empty());
    }
}

TEST_CASE("trim right") {
    static_assert(strings::trim_right("\n\t\rfoobar\r\t\n"sv, "\t\r\n\v"sv) == "\n\t\rfoobar"sv);
    static_assert(strings::trim_right("\n\t\r\r\t\n"sv, "\t\r\n\v"sv).empty());
    static_assert(strings::trim_right("\n\t\rfoobar"sv, "\t\r\n\v"sv) == "\n\t\rfoobar"sv);
    static_assert(strings::trim_right(""sv, "\t\r\n\v"sv).empty());
}

TEST_CASE("trim right inplace") {
    SUBCASE("remove matched in right side") {
        std::string str{"\n\t\rfoobar\r\t\n"};
        strings::trim_right_inplace(str, "\t\r\n\v"sv);
        CHECK_EQ(str, "\n\t\rfoobar");
    }

    SUBCASE("every character in str matches") {
        std::string str{"\n\t\r\r\t\n"};
        strings::trim_right_inplace(str, "\t\r\n\v"sv);
        CHECK(str.empty());
    }

    SUBCASE("none character in str matches") {
        std::string str{"\n\t\rfoobar"};
        strings::trim_right_inplace(str, "\t\r\n\v"sv);
        CHECK_EQ(str, "\n\t\rfoobar");
    }

    SUBCASE("empty string") {
        std::string str;
        strings::trim_right_inplace(str, "\t\r\n\v"sv);
        CHECK(str.empty());
    }
}

TEST_CASE("trim both sides") {
    static_assert(strings::trim("\n\t\rfoobar\r\t\n"sv, "\t\r\n\v"sv) == "foobar"sv);
    static_assert(strings::trim("\n\t\r\r\t\n"sv, "\t\r\n\v"sv).empty());
    static_assert(strings::trim("foobar"sv, "\t\r\n\v"sv) == "foobar"sv);
    static_assert(strings::trim(""sv, "\t\r\n\v"sv).empty());
}

TEST_CASE("trim both sides inplace") {
    SUBCASE("remove matched in both sides") {
        std::string str{"\n\t\rfoobar\r\t\n"};
        strings::trim_inplace(str, "\t\r\n\v"sv);
        CHECK_EQ(str, "foobar");
    }

    SUBCASE("every character matches") {
        std::string str{"\n\t\r\r\t\n"};
        strings::trim_inplace(str, "\r\n\t");
        CHECK(str.empty());
    }

    SUBCASE("none matches") {
        std::string str{"foobar"};
        strings::trim_inplace(str, "\t\r\n\v");
        CHECK_EQ(str, "foobar");
    }

    SUBCASE("empty string") {
        std::string str;
        strings::trim_inplace(str, "\t\r\n\v");
        CHECK(str.empty());
    }
}

TEST_SUITE_END();

} // namespace
