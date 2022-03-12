// Copyright (c) 2022 Kingsley Chen <kingsamchen at gmail dot com>
// This file is subject to the terms of license that can be found
// in the LICENSE file.

#include <deque>
#include <forward_list>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <stack>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "doctest/doctest.h"

#include "esl/strings.h"
#include "tests/stringification.h"

namespace detail = esl::strings::detail;
namespace strings = esl::strings;

namespace {

struct dummy_delimiter {
    std::size_t size() const noexcept {
        (void)this;
        return 0;
    }

    std::size_t find(std::string_view, std::size_t) const noexcept {
        (void)this;
        return std::string_view::npos;
    }
};

bool allow_any(std::string_view) {
    return true;
}

using allow_any_t = std::decay_t<decltype(allow_any)>;

bool not_empty(std::string_view s) {
    return !s.empty();
}

using not_empty_t = std::decay_t<decltype(not_empty)>;

TEST_SUITE_BEGIN("strings/split");

TEST_CASE("split iterator") {
    SUBCASE("type traits") {
        using iter_t = detail::split_iterator<dummy_delimiter, allow_any_t>;
        static_assert(std::is_same_v<std::iterator_traits<iter_t>::iterator_category,
                                     std::forward_iterator_tag>);
    }

    SUBCASE("compare against end iterator") {
        using iter_t = detail::split_iterator<strings::by_string, allow_any_t>;

        iter_t end1;
        iter_t end2;
        CHECK_EQ(end1, end2);

        iter_t iter("abc\ndef", 0, strings::by_string("\n"), allow_any);
        iter_t it_end("abc\ndef");
        CHECK_NE(iter, end1);
        CHECK_NE(iter, it_end);
        CHECK_EQ(it_end, end1);
        CHECK_EQ(it_end, end2);
    }

    SUBCASE("compare iterators") {
        using iter_t = detail::split_iterator<strings::by_string, allow_any_t>;
        iter_t it("abc\ndef\n", 0, strings::by_string("\n"), allow_any);
        CHECK_EQ(*it, "abc");
        ++it;
        CHECK_EQ(*it, "def");
        ++it;
        CHECK_NE(it, iter_t{});
        ++it;
        CHECK_EQ(it, iter_t{});
    }

    SUBCASE("iterate by string delimiter and allow any token") {
        using iter_t = detail::split_iterator<strings::by_string, allow_any_t>;
        strings::by_string delim("\r\n");
        std::vector<std::string_view> tokens;

        SUBCASE("normal case") {
            std::string str{"abc\r\ndef\r\nfoobar"};
            for (iter_t it(str, 0, delim, allow_any); it != iter_t{}; ++it) {
                tokens.push_back(*it);
            }
            REQUIRE_EQ(tokens.size(), 3);
            CHECK_EQ(tokens, std::vector<std::string_view>{"abc", "def", "foobar"});
        }

        SUBCASE("empty input") {
            std::string_view sv{};

            for (iter_t it(sv, 0, delim, allow_any); it != iter_t{}; ++it) {
                tokens.push_back(*it);
            }
            REQUIRE_EQ(tokens.size(), 1);
            CHECK_EQ(tokens[0], std::string_view{});
        }

        SUBCASE("input doesn't contain delim") {
            std::string_view sv = "foobar";
            for (iter_t it(sv, 0, delim, allow_any); it != iter_t{}; ++it) {
                tokens.push_back(*it);
            }
            REQUIRE_EQ(tokens.size(), 1);
            CHECK_EQ(tokens[0], std::string_view{"foobar"});
        }

        SUBCASE("input contains only delim") {
            std::string_view sv = "\r\n";
            for (iter_t it(sv, 0, delim, allow_any); it != iter_t{}; ++it) {
                tokens.push_back(*it);
            }
            REQUIRE_EQ(tokens.size(), 2);
            CHECK_EQ(tokens, std::vector<std::string_view>{{}, {}});
        }

        SUBCASE("multipass support") {
            std::string str{"abc\r\ndef\r\nfoobar"};
            iter_t it(str, 0, delim, allow_any);
            // `it` is copied as the parameter.
            auto distance = std::distance(it, iter_t{});
            CHECK_EQ(distance, 3);
            // `it` must keep intact and still can increment.
            CHECK_EQ(*it, "abc");
            auto end_it = std::next(it, distance);
            CHECK_EQ(end_it, iter_t{});
            CHECK_NE(end_it, it);
        }
    }

    SUBCASE("use predicate to filter tokens") {
        using iter_t = detail::split_iterator<strings::by_string, not_empty_t>;
        std::string str{"abc\r\n\r\ndef\r\n\r\n"};
        std::vector<std::string_view> tokens;
        iter_t it(str, 0, strings::by_string("\r\n"), not_empty);
        size_t incr_count = 0;
        for (; it != iter_t{}; ++it, ++incr_count) {
            tokens.push_back(*it);
        }
        REQUIRE_EQ(tokens.size(), 2);
        CHECK_EQ(tokens, std::vector<std::string_view>{"abc", "def"});
    }

    SUBCASE("split by any character") {
        using iter_t = detail::split_iterator<strings::by_any_char, not_empty_t>;
        std::string str{"abc\r\n\r\ndef\n\r\n\r"};
        std::vector<std::string_view> tokens;
        iter_t it(str, 0, strings::by_any_char("\r\n"), not_empty);
        size_t incr_count = 0;
        for (; it != iter_t{}; ++it, ++incr_count) {
            tokens.push_back(*it);
        }
        REQUIRE_EQ(tokens.size(), 2);
        CHECK_EQ(tokens, std::vector<std::string_view>{"abc", "def"});
    }
}

TEST_CASE("split view type constraints") {
    SUBCASE("most commonly used containers should support") {
        static_assert(detail::can_construct_container_v<std::vector<std::string>>);
        static_assert(detail::can_construct_container_v<std::deque<std::string>>);
        static_assert(detail::can_construct_container_v<std::list<std::string>>);
        static_assert(detail::can_construct_container_v<std::set<std::string>>);
        static_assert(detail::can_construct_container_v<std::unordered_set<std::string>>);
    }

    SUBCASE("containers that not supported") {
        // value_type of map-like containers is usually of pair<key_t, value_t>
        // cannot be constructed from `std::string_view`.
        static_assert(!detail::can_construct_container_v<std::map<std::string, std::string>>);
        static_assert(
                !detail::can_construct_container_v<std::unordered_map<std::string, std::string>>);

        // These containers don't offer `insert()` function.
        static_assert(!detail::can_construct_container_v<std::forward_list<std::string>>);
        static_assert(!detail::can_construct_container_v<std::initializer_list<std::string>>);
        static_assert(!detail::can_construct_container_v<std::array<std::string, 1>>);
        static_assert(!detail::can_construct_container_v<std::stack<std::string>>);
        static_assert(!detail::can_construct_container_v<std::queue<std::string>>);

        // Cases of wrong types.
        static_assert(!detail::can_construct_container_v<std::string>);
        static_assert(!detail::can_construct_container_v<std::string_view>);
    }
}

TEST_CASE("split view to container usages") {
    using split_view =
            detail::split_view<std::string_view, strings::by_any_char, strings::skip_empty>;
    split_view splitter("-foo--bar--baz--hello--world-", strings::by_any_char{"-"}, {});

    auto vec_sv = splitter.to<std::vector<std::string_view>>();
    CHECK_EQ(vec_sv, std::vector<std::string_view>{"foo", "bar", "baz", "hello", "world"});

    auto vec_str = splitter.to<std::vector<std::string>>();
    CHECK_EQ(vec_str, std::vector<std::string>{"foo", "bar", "baz", "hello", "world"});
}

TEST_CASE_TEMPLATE("split view with StringTypes", StringType, std::string_view, std::string) {
    using split_view = detail::split_view<StringType, strings::by_any_char, strings::skip_empty>;
    split_view splitter("-foo--bar--baz--hello--world-", strings::by_any_char{"-"}, {});

    // Because `split_view` now is a dependent type because of `StringType` argument.
    // The `template` keyword before `to()` is a must.

    SUBCASE("to a vector of string") {
        auto vec = splitter.template to<std::vector<std::string>>();
        CHECK_EQ(vec, std::vector<std::string>{"foo", "bar", "baz", "hello", "world"});
    }

    SUBCASE("to a deque of string") {
        auto deq = splitter.template to<std::deque<std::string>>();
        CHECK_EQ(deq, std::deque<std::string>{"foo", "bar", "baz", "hello", "world"});
    }

    SUBCASE("to a list of string") {
        auto list = splitter.template to<std::list<std::string>>();
        CHECK_EQ(list, std::list<std::string>{"foo", "bar", "baz", "hello", "world"});
    }

    SUBCASE("to a set/unordered_set of string") {
        auto set = splitter.template to<std::set<std::string>>();
        CHECK_EQ(set, std::set<std::string>{"foo", "bar", "baz", "hello", "world"});

        auto hashset = splitter.template to<std::unordered_set<std::string>>();
        CHECK_EQ(hashset, std::unordered_set<std::string>{"foo", "bar", "baz", "hello", "world"});
    }
}

TEST_CASE("Delimiter selector") {
    SUBCASE("a delimiter itself will be selected") {
        static_assert(std::is_same_v<detail::select_delimiter<strings::by_string>::type,
                                     strings::by_string>);
        static_assert(std::is_same_v<detail::select_delimiter<strings::by_char>::type,
                                     strings::by_char>);
        static_assert(std::is_same_v<detail::select_delimiter<strings::by_any_char>::type,
                                     strings::by_any_char>);
    }

    // Since a `Delimiter` is always passed by value, deduced type will not be qualified
    // by cv or/and ref.
    SUBCASE("deduce a delimiter from given value") {
        static_assert(std::is_same_v<detail::select_delimiter<char>::type,
                                     strings::by_char>);
        static_assert(std::is_same_v<detail::select_delimiter<char*>::type,
                                     strings::by_string>);
        static_assert(std::is_same_v<detail::select_delimiter<const char*>::type,
                                     strings::by_string>);
        static_assert(std::is_same_v<detail::select_delimiter<std::string_view>::type,
                                     strings::by_string>);
        static_assert(std::is_same_v<detail::select_delimiter<std::string>::type,
                                     strings::by_string>);
    }
}

TEST_CASE("split functions") {
    SUBCASE("normal usages") {
        SUBCASE("auto deduce as by_char delimiter") {
            auto vec = strings::split("foo-bar-baz-", '-').to<std::vector<std::string_view>>();
            CHECK_EQ(vec, std::vector<std::string_view>{"foo", "bar", "baz", ""});
        }

        SUBCASE("auto deduce as by_string delimiter") {
            auto vec = strings::split("foo:=bar:=baz:===", ":=")
                               .to<std::vector<std::string_view>>();
            CHECK_EQ(vec, std::vector<std::string_view>{"foo", "bar", "baz", "=="});
        }

        SUBCASE("explicitly specify delimiter") {
            auto vec = strings::split("foo:=bar:=baz:===", strings::by_any_char(":="))
                               .to<std::vector<std::string_view>>();
            CHECK_EQ(vec,
                     std::vector<std::string_view>{"foo", "", "bar", "", "baz", "", "", "", ""});
        }
    }

    SUBCASE("source text is a rvalue of std::string") {
        // Lifetime of the input text ends as the lifetime of split_view ends,
        // which happens when function to<>() returns.
        // Thus we must use std::string for each token part.

        SUBCASE("a prvalue case") {
            auto vec = strings::split(std::string("foo-bar-baz-"), '-')
                               .to<std::vector<std::string>>();
            CHECK_EQ(vec, std::vector<std::string>{"foo", "bar", "baz", ""});
        }

        SUBCASE("an xvalue case") {
            auto str = std::string("foo-bar-baz-");
            auto vec = strings::split(std::move(str), '-')
                               .to<std::vector<std::string>>();
            CHECK_EQ(vec, std::vector<std::string>{"foo", "bar", "baz", ""});
        }
    }

    SUBCASE("specify another predicate") {
        SUBCASE("normal case") {
            auto vec = strings::split("foo:=bar:=baz:===",
                                      strings::by_any_char(":="),
                                      strings::skip_empty{})
                               .to<std::vector<std::string_view>>();
            CHECK_EQ(vec, std::vector<std::string_view>{"foo", "bar", "baz"});
        }

        SUBCASE("source is a prvalue") {
            auto vec = strings::split(std::string("foo:=bar:=baz:==="),
                                      strings::by_any_char(":="),
                                      strings::skip_empty{})
                               .to<std::vector<std::string>>();
            CHECK_EQ(vec, std::vector<std::string>{"foo", "bar", "baz"});
        }

        SUBCASE("predicate allow function itself") {
            auto vec = strings::split("foo:=bar:=baz:===", strings::by_any_char(":="), not_empty)
                               .to<std::vector<std::string_view>>();
            CHECK_EQ(vec, std::vector<std::string_view>{"foo", "bar", "baz"});
        }
    }
}

TEST_SUITE_END();

} // namespace
