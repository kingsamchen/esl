// Copyright (c) 2021 Kingsley Chen <kingsamchen at gmail dot com>
// This file is subject to the terms of license that can be found
// in the LICENSE file.

#include <deque>
#include <forward_list>
#include <iterator>
#include <list>
#include <set>
#include <string>
#include <type_traits>
#include <unordered_set>
#include <vector>

#include "doctest/doctest.h"

#include "esl/strings.h"

namespace strings = esl::strings;

namespace strings_test {

struct sizable_foo {
    [[nodiscard]] std::size_t size() const noexcept {
        (void)this;
        return 0;
    }

    [[nodiscard]] const char* data() const noexcept {
        (void)this;
        return nullptr;
    }
};

} // namespace strings_test

template<>
struct strings::detail::is_sizable_str<strings_test::sizable_foo> : std::true_type {};

namespace {

template<typename T>
inline constexpr bool is_sizable_str_v = strings::detail::is_sizable_str<T>::value;

template<typename Iterator>
inline constexpr bool is_sizable_str_multipass_range_v =
        strings::detail::is_sizable_str_multipass_range<Iterator>::value;

struct foo {};

template<typename T, typename = void>
struct can_append : std::false_type {};

template<typename T>
struct can_append<T, std::void_t<decltype(to_append(T{}, std::declval<std::string&>()))>>
    : std::true_type {};

TEST_SUITE_BEGIN("strings/join");

TEST_CASE("is_sizable_str type traits") {
    SUBCASE("predefined for standard library") {
        // Currently supported
        CHECK(is_sizable_str_v<std::string>);
        CHECK(is_sizable_str_v<std::string_view>);

        // Should fail.
        CHECK_FALSE(is_sizable_str_v<char>);
        CHECK_FALSE(is_sizable_str_v<const char>);
        CHECK_FALSE(is_sizable_str_v<char*>);
        CHECK_FALSE(is_sizable_str_v<const char*>);
        CHECK_FALSE(is_sizable_str_v<int>);
        CHECK_FALSE(is_sizable_str_v<std::vector<char>>);
        CHECK_FALSE(is_sizable_str_v<foo>);
    }

    SUBCASE("allow customization for sizable types") {
        CHECK(is_sizable_str_v<strings_test::sizable_foo>);
    }
}

TEST_CASE("is_sizable_str_multipass_range type traits") {
    SUBCASE("almost every common seen containers are qualified") {
        using vec_str_t = std::vector<std::string>;
        CHECK(is_sizable_str_multipass_range_v<vec_str_t::iterator>);
        CHECK(is_sizable_str_multipass_range_v<vec_str_t::const_iterator>);
        CHECK(is_sizable_str_multipass_range_v<vec_str_t::reverse_iterator>);

        using ary_str_iterator_t = std::string*;
        CHECK(is_sizable_str_multipass_range_v<ary_str_iterator_t>);

        using deq_str_t = std::deque<std::string>;
        CHECK(is_sizable_str_multipass_range_v<deq_str_t::iterator>);
        CHECK(is_sizable_str_multipass_range_v<deq_str_t::const_iterator>);

        using set_str_t = std::set<std::string>;
        CHECK(is_sizable_str_multipass_range_v<set_str_t::iterator>);
        CHECK(is_sizable_str_multipass_range_v<set_str_t::const_iterator>);

        using hash_set_str_t = std::unordered_set<std::string>;
        CHECK(is_sizable_str_multipass_range_v<hash_set_str_t::iterator>);
        CHECK(is_sizable_str_multipass_range_v<hash_set_str_t::const_iterator>);

        using singly_list_str_t = std::forward_list<std::string>;
        CHECK(is_sizable_str_multipass_range_v<singly_list_str_t::iterator>);
        CHECK(is_sizable_str_multipass_range_v<singly_list_str_t::const_iterator>);

        using list_str_t = std::list<std::string>;
        CHECK(is_sizable_str_multipass_range_v<list_str_t::iterator>);
        CHECK(is_sizable_str_multipass_range_v<list_str_t::const_iterator>);
    }

    SUBCASE("not hold for non-sizable str") {
        using vec_psz_t = std::vector<const char*>;
        CHECK_FALSE(is_sizable_str_multipass_range_v<vec_psz_t::iterator>);
        CHECK_FALSE(is_sizable_str_multipass_range_v<vec_psz_t::const_iterator>);
        CHECK_FALSE(is_sizable_str_multipass_range_v<vec_psz_t::reverse_iterator>);
    }

    SUBCASE("not hold for input iterator") {
        using input_str_iter = std::istream_iterator<std::string>;
        REQUIRE(std::is_same_v<input_str_iter::value_type, std::string>);
        CHECK_FALSE(is_sizable_str_multipass_range_v<input_str_iter>);
    }
}

TEST_CASE("to_append overloads") {
    std::string out;

    SUBCASE("append a sizable string") {
        using namespace std::string_literals;
        using namespace std::string_view_literals;

        strings::detail::to_append("foo-"s, out);
        CHECK_EQ("foo-", out);

        strings::detail::to_append("bar-baz"sv, out);
        CHECK_EQ("foo-bar-baz", out);
    }

    SUBCASE("append a raw c-style string") {
        const char* pcsz = "foo-";
        strings::detail::to_append(pcsz, out);
        CHECK_EQ("foo-", out);

        char sz[] = "bar-";
        char* psz = sz;
        strings::detail::to_append(psz, out);
        CHECK_EQ("foo-bar-", out);

        const char* const cpcsz = pcsz;
        strings::detail::to_append(cpcsz, out);
        CHECK_EQ("foo-bar-foo-", out);

        CHECK_FALSE(can_append<int*>::value);
        CHECK_FALSE(can_append<void*>::value);
    }

    SUBCASE("append a char") {
        constexpr char c1 = 'X';
        strings::detail::to_append(c1, out);
        CHECK_EQ("X", out);

        constexpr const char c2 = 'Y';
        strings::detail::to_append(c2, out);
        CHECK_EQ("XY", out);

        CHECK_FALSE(can_append<int>::value);
        CHECK_FALSE(can_append<double>::value);
    }
}

TEST_CASE("trivial api examples") {
    SUBCASE("most fundamental api") {
        const std::vector<std::string> strs{"foo", "bar", "baz"};
        std::string out;
        strings::join(strs.begin(), strs.end(), "-", out);
        CHECK_EQ("foo-bar-baz", out);
    }

    SUBCASE("sequence of std::string") {
        const std::vector<std::string> strs{"foo", "bar", "baz"};
        CHECK_EQ("foo-bar-baz", strings::join(strs, "-"));
    }

    SUBCASE("sequence of std::string_view") {
        const std::vector<std::string_view> strs{"foo", "bar", "baz"};
        CHECK_EQ("foo-bar-baz", strings::join(strs, "-"));
    }

    SUBCASE("sequence of const char*") {
        const std::vector<const char*> strs{"foo", "bar", "baz"};
        CHECK_EQ("foo-bar-baz", strings::join(strs, "-"));
    }

    SUBCASE("sequence of char*") {
        char sz_a[] = "foo";
        char sz_b[] = "bar";
        char sz_c[] = "baz";
        const std::vector<char*> strs{sz_a, sz_b, sz_c};
        CHECK_EQ("foo-bar-baz", strings::join(strs, "-"));
    }

    SUBCASE("sequence of char") {
        const std::string chs = "hello";
        CHECK_EQ("h-e-l-l-o", strings::join(chs, "-"));
    }

    SUBCASE("passed as a initializer list") {
        CHECK_EQ("foo-bar-baz", strings::join({"foo", "bar", "baz"}, "-"));
    }

    SUBCASE("passed as an array") {
        constexpr const char* strs[]{"foo", "bar", "baz"};
        CHECK_EQ("foo-bar-baz", strings::join(strs, "-"));
    }
}

TEST_CASE("trivial api edge cases") {
    SUBCASE("empty sequence") {
        const std::vector<std::string> strs;

        SUBCASE("clear original for out apis") {
            std::string s1 = "-";
            strings::join(strs, "-", s1);
            CHECK_EQ("", s1);
        }

        SUBCASE("always returns an empty string") {
            CHECK_EQ("", strings::join(strs, "-"));
        }
    }

    SUBCASE("only one element in sequence returns the element without sep") {
        const std::vector<std::string> strs{"foo"};
        CHECK_EQ("foo", strings::join(strs, "-"));
    }

    SUBCASE("when the only element is an empty string") {
        const std::vector<std::string> strs{""};
        CHECK_EQ("", strings::join(strs, "-"));
    }

    SUBCASE("a sequence of two empty strings") {
        const std::vector<std::string> strs{"", ""};
        CHECK_EQ("-", strings::join(strs, "-"));
    }

    SUBCASE("one of two elements is empty") {
        const std::vector<std::string> strs{"", "foo"};
        CHECK_EQ("-foo", strings::join(strs, "-"));
    }
}

TEST_CASE("appender api examples") {
    SUBCASE("a sequence of pair") {
        // NOLINTNEXTLINE(readability-magic-numbers)
        const std::vector<std::pair<char, int>> seq{{'A', 65}, {'B', 66}, {'C', 67}};
        auto str = strings::join(seq, " ", [](const auto& e, std::string& out) {
            out.append(1, e.first).append("->").append(std::to_string(e.second));
        });
        CHECK_EQ("A->65 B->66 C->67", str);
    }

    SUBCASE("a sequence of ints") {
        const std::vector<int> ints{1, 3, 5, 7, 9};
        CHECK_EQ("1 - 3 - 5 - 7 - 9",
                 strings::join(ints, " - ", [](int n, auto& o) { o.append(std::to_string(n)); }));
    }
}

TEST_SUITE_END();

} // namespace
