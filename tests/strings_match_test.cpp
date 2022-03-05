// Copyright (c) 2022 Kingsley Chen <kingsamchen at gmail dot com>
// This file is subject to the terms of license that can be found
// in the LICENSE file.

#include <string>
#include <string_view>

#include "doctest/doctest.h"

#include "esl/strings.h"

namespace strings = esl::strings;

namespace {

// clang-format off

constexpr int k_table_size = 256;

constexpr char to_lower_table[k_table_size] = {
    '\x00', '\x01', '\x02', '\x03', '\x04', '\x05', '\x06', '\x07',
    '\x08', '\x09', '\x0a', '\x0b', '\x0c', '\x0d', '\x0e', '\x0f',
    '\x10', '\x11', '\x12', '\x13', '\x14', '\x15', '\x16', '\x17',
    '\x18', '\x19', '\x1a', '\x1b', '\x1c', '\x1d', '\x1e', '\x1f',
    '\x20', '\x21', '\x22', '\x23', '\x24', '\x25', '\x26', '\x27',
    '\x28', '\x29', '\x2a', '\x2b', '\x2c', '\x2d', '\x2e', '\x2f',
    '\x30', '\x31', '\x32', '\x33', '\x34', '\x35', '\x36', '\x37',
    '\x38', '\x39', '\x3a', '\x3b', '\x3c', '\x3d', '\x3e', '\x3f',
    '\x40',    'a',    'b',    'c',    'd',    'e',    'f',    'g',
       'h',    'i',    'j',    'k',    'l',    'm',    'n',    'o',
       'p',    'q',    'r',    's',    't',    'u',    'v',    'w',
       'x',    'y',    'z', '\x5b', '\x5c', '\x5d', '\x5e', '\x5f',
    '\x60', '\x61', '\x62', '\x63', '\x64', '\x65', '\x66', '\x67',
    '\x68', '\x69', '\x6a', '\x6b', '\x6c', '\x6d', '\x6e', '\x6f',
    '\x70', '\x71', '\x72', '\x73', '\x74', '\x75', '\x76', '\x77',
    '\x78', '\x79', '\x7a', '\x7b', '\x7c', '\x7d', '\x7e', '\x7f',
    '\x80', '\x81', '\x82', '\x83', '\x84', '\x85', '\x86', '\x87',
    '\x88', '\x89', '\x8a', '\x8b', '\x8c', '\x8d', '\x8e', '\x8f',
    '\x90', '\x91', '\x92', '\x93', '\x94', '\x95', '\x96', '\x97',
    '\x98', '\x99', '\x9a', '\x9b', '\x9c', '\x9d', '\x9e', '\x9f',
    '\xa0', '\xa1', '\xa2', '\xa3', '\xa4', '\xa5', '\xa6', '\xa7',
    '\xa8', '\xa9', '\xaa', '\xab', '\xac', '\xad', '\xae', '\xaf',
    '\xb0', '\xb1', '\xb2', '\xb3', '\xb4', '\xb5', '\xb6', '\xb7',
    '\xb8', '\xb9', '\xba', '\xbb', '\xbc', '\xbd', '\xbe', '\xbf',
    '\xc0', '\xc1', '\xc2', '\xc3', '\xc4', '\xc5', '\xc6', '\xc7',
    '\xc8', '\xc9', '\xca', '\xcb', '\xcc', '\xcd', '\xce', '\xcf',
    '\xd0', '\xd1', '\xd2', '\xd3', '\xd4', '\xd5', '\xd6', '\xd7',
    '\xd8', '\xd9', '\xda', '\xdb', '\xdc', '\xdd', '\xde', '\xdf',
    '\xe0', '\xe1', '\xe2', '\xe3', '\xe4', '\xe5', '\xe6', '\xe7',
    '\xe8', '\xe9', '\xea', '\xeb', '\xec', '\xed', '\xee', '\xef',
    '\xf0', '\xf1', '\xf2', '\xf3', '\xf4', '\xf5', '\xf6', '\xf7',
    '\xf8', '\xf9', '\xfa', '\xfb', '\xfc', '\xfd', '\xfe', '\xff',
};

constexpr char to_upper_table[k_table_size] = {
    '\x00', '\x01', '\x02', '\x03', '\x04', '\x05', '\x06', '\x07',
    '\x08', '\x09', '\x0a', '\x0b', '\x0c', '\x0d', '\x0e', '\x0f',
    '\x10', '\x11', '\x12', '\x13', '\x14', '\x15', '\x16', '\x17',
    '\x18', '\x19', '\x1a', '\x1b', '\x1c', '\x1d', '\x1e', '\x1f',
    '\x20', '\x21', '\x22', '\x23', '\x24', '\x25', '\x26', '\x27',
    '\x28', '\x29', '\x2a', '\x2b', '\x2c', '\x2d', '\x2e', '\x2f',
    '\x30', '\x31', '\x32', '\x33', '\x34', '\x35', '\x36', '\x37',
    '\x38', '\x39', '\x3a', '\x3b', '\x3c', '\x3d', '\x3e', '\x3f',
    '\x40', '\x41', '\x42', '\x43', '\x44', '\x45', '\x46', '\x47',
    '\x48', '\x49', '\x4a', '\x4b', '\x4c', '\x4d', '\x4e', '\x4f',
    '\x50', '\x51', '\x52', '\x53', '\x54', '\x55', '\x56', '\x57',
    '\x58', '\x59', '\x5a', '\x5b', '\x5c', '\x5d', '\x5e', '\x5f',
    '\x60',    'A',    'B',    'C',    'D',    'E',    'F',    'G',
       'H',    'I',    'J',    'K',    'L',    'M',    'N',    'O',
       'P',    'Q',    'R',    'S',    'T',    'U',    'V',    'W',
       'X',    'Y',    'Z', '\x7b', '\x7c', '\x7d', '\x7e', '\x7f',
    '\x80', '\x81', '\x82', '\x83', '\x84', '\x85', '\x86', '\x87',
    '\x88', '\x89', '\x8a', '\x8b', '\x8c', '\x8d', '\x8e', '\x8f',
    '\x90', '\x91', '\x92', '\x93', '\x94', '\x95', '\x96', '\x97',
    '\x98', '\x99', '\x9a', '\x9b', '\x9c', '\x9d', '\x9e', '\x9f',
    '\xa0', '\xa1', '\xa2', '\xa3', '\xa4', '\xa5', '\xa6', '\xa7',
    '\xa8', '\xa9', '\xaa', '\xab', '\xac', '\xad', '\xae', '\xaf',
    '\xb0', '\xb1', '\xb2', '\xb3', '\xb4', '\xb5', '\xb6', '\xb7',
    '\xb8', '\xb9', '\xba', '\xbb', '\xbc', '\xbd', '\xbe', '\xbf',
    '\xc0', '\xc1', '\xc2', '\xc3', '\xc4', '\xc5', '\xc6', '\xc7',
    '\xc8', '\xc9', '\xca', '\xcb', '\xcc', '\xcd', '\xce', '\xcf',
    '\xd0', '\xd1', '\xd2', '\xd3', '\xd4', '\xd5', '\xd6', '\xd7',
    '\xd8', '\xd9', '\xda', '\xdb', '\xdc', '\xdd', '\xde', '\xdf',
    '\xe0', '\xe1', '\xe2', '\xe3', '\xe4', '\xe5', '\xe6', '\xe7',
    '\xe8', '\xe9', '\xea', '\xeb', '\xec', '\xed', '\xee', '\xef',
    '\xf0', '\xf1', '\xf2', '\xf3', '\xf4', '\xf5', '\xf6', '\xf7',
    '\xf8', '\xf9', '\xfa', '\xfb', '\xfc', '\xfd', '\xfe', '\xff',
};

// clang-format on

TEST_SUITE_BEGIN("strings/match");

TEST_CASE("ascii case switch") {
    SUBCASE("switch A-Z to a-z while retaining other chars") {
        for (int i = 0; i < k_table_size; ++i) {
            auto c = static_cast<unsigned char>(i);
            CHECK_EQ(strings::detail::ascii_to_lower(static_cast<char>(c)), to_lower_table[c]);
        }
    }

    SUBCASE("switch a-z to A-Z while retaining other chars") {
        for (int i = 0; i < k_table_size; ++i) {
            auto c = static_cast<unsigned char>(i);
            CHECK_EQ(strings::detail::ascii_to_upper(static_cast<char>(c)), to_upper_table[c]);
        }
    }

    SUBCASE("compile-time support") {
        static_assert(strings::detail::ascii_to_upper('a') == 'A');
        static_assert(strings::detail::ascii_to_lower('A') == 'a');
    }
}

TEST_CASE("compare string and ignore ascii cases") {
    SUBCASE("compare first n chars case-insensitive") {
        SUBCASE("equal") {
            CHECK_EQ(strings::detail::compare_n_ignore_ascii_case("foobar", "FOOBAR", 6), 0);
            CHECK_EQ(strings::detail::compare_n_ignore_ascii_case("foobar", "FOOBAR", 6), 0);
            CHECK_EQ(strings::detail::compare_n_ignore_ascii_case("foobar", "FOOBAZ", 5), 0);
        }

        SUBCASE("lexically before") {
            CHECK_LT(strings::detail::compare_n_ignore_ascii_case("bar", "baz", 3), 0);
            CHECK_LT(strings::detail::compare_n_ignore_ascii_case("BAR", "baz", 3), 0);
            CHECK_LT(strings::detail::compare_n_ignore_ascii_case("bar", "BAZ", 3), 0);
            CHECK_LT(strings::detail::compare_n_ignore_ascii_case("barz", "baz", 3), 0);
        }

        SUBCASE("lexically after") {
            CHECK_GT(strings::detail::compare_n_ignore_ascii_case("baz", "bar", 3), 0);
            CHECK_GT(strings::detail::compare_n_ignore_ascii_case("BAZ", "bar", 3), 0);
            CHECK_GT(strings::detail::compare_n_ignore_ascii_case("baza", "barz", 3), 0);
        }
    }

    SUBCASE("check if two strings equal") {
        CHECK(strings::equals_ignore_ascii_case("foobar", "foobar"));
        CHECK(strings::equals_ignore_ascii_case("foobar", "FOOBAR"));
        CHECK(strings::equals_ignore_ascii_case("fooBAR", "FOObar"));
        CHECK_FALSE(strings::equals_ignore_ascii_case("foobar", "foo"));
        CHECK_FALSE(strings::equals_ignore_ascii_case("foo", "foobaz"));
        CHECK_FALSE(strings::equals_ignore_ascii_case("foobar", "foobaz"));
    }

    SUBCASE("compile-time support") {
        static_assert(strings::equals_ignore_ascii_case("fooBAR", "FOObar"));
        static_assert(!strings::equals_ignore_ascii_case("foobar", "foobaz"));
    }
}

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

TEST_CASE("starts with ignore ascii case") {
    using namespace std::string_literals;
    using namespace std::string_view_literals;

    SUBCASE("normal cases") {
        std::string_view sv = "FOOBAR";
        CHECK(strings::starts_with_ignore_ascii_case(sv, sv));
        CHECK(strings::starts_with_ignore_ascii_case(sv, "foo"));
        CHECK(strings::starts_with_ignore_ascii_case(sv, ""sv));
        CHECK_FALSE(strings::starts_with_ignore_ascii_case(sv, "foobaz"));

        std::string_view esv;
        CHECK(strings::starts_with_ignore_ascii_case(esv, ""));
        CHECK_FALSE(strings::starts_with_ignore_ascii_case(esv, sv));

        std::string str = "123\0abc"s;
        CHECK(strings::starts_with_ignore_ascii_case(str, str));
        CHECK(strings::starts_with_ignore_ascii_case(str, "123\0"s));
        CHECK(strings::starts_with_ignore_ascii_case(str, ""sv));
        CHECK_FALSE(strings::starts_with_ignore_ascii_case(str, "abc"));
    }

    SUBCASE("when |str| < |prefix|") {
        std::string str = "FOO";
        CHECK_FALSE(strings::starts_with_ignore_ascii_case(str, "foobar"));
    }

    SUBCASE("compile time support") {
        static constexpr std::string_view text = "foobar";
        static constexpr std::string_view pref = "FOO";
        static_assert(strings::starts_with_ignore_ascii_case(text, pref));
    }
}

TEST_CASE("ends with ignore ascii case") {
    using namespace std::string_literals;
    using namespace std::string_view_literals;

    SUBCASE("normal cases") {
        std::string_view sv = "FOOBAR";
        CHECK(strings::ends_with_ignore_ascii_case(sv, sv));
        CHECK(strings::ends_with_ignore_ascii_case(sv, "bar"));
        CHECK(strings::ends_with_ignore_ascii_case(sv, ""sv));
        CHECK_FALSE(strings::ends_with_ignore_ascii_case(sv, "foobaz"));

        std::string_view esv;
        CHECK(strings::ends_with_ignore_ascii_case(esv, ""));
        CHECK_FALSE(strings::ends_with_ignore_ascii_case(esv, sv));

        std::string str = "123\0abc"s;
        CHECK(strings::ends_with_ignore_ascii_case(str, str));
        CHECK(strings::ends_with_ignore_ascii_case(str, "\0ABC"s));
        CHECK(strings::ends_with_ignore_ascii_case(str, ""sv));
        CHECK_FALSE(strings::ends_with_ignore_ascii_case(str, "0ABC"));
    }

    SUBCASE("when |str| < |suffix|") {
        std::string str = "bar";
        CHECK_FALSE(strings::ends_with_ignore_ascii_case(str, "FooBar"));
    }

    SUBCASE("compile time support") {
        static constexpr std::string_view text = "foobar";
        static constexpr std::string_view pref = "Bar";
        static_assert(strings::ends_with_ignore_ascii_case(text, pref));
    }
}

TEST_SUITE_END();

} // namespace
