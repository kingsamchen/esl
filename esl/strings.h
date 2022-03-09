// Copyright (c) 2022 Kingsley Chen <kingsamchen at gmail dot com>
// This file is subject to the terms of license that can be found
// in the LICENSE file.

#pragma once

#ifndef ESL_STRINGS_H_
#define ESL_STRINGS_H_

#include <cassert>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

#include "esl/detail/strings_join.h"
#include "esl/detail/strings_split.h"

namespace esl::strings {
namespace detail {

constexpr char ascii_to_lower(char ch) noexcept {
    return 'A' <= ch && ch <= 'Z' ? static_cast<char>('a' + ch - 'A') : ch;
}

constexpr char ascii_to_upper(char ch) noexcept {
    return 'a' <= ch && ch <= 'z' ? static_cast<char>('A' + ch - 'a') : ch;
}

constexpr int compare_n_ignore_ascii_case(std::string_view s1,
                                          std::string_view s2,
                                          std::size_t len) noexcept {
    assert(len <= std::min(s1.size(), s2.size()));

    for (std::size_t i = 0; i < len; ++i) {
        auto delta = static_cast<int>(static_cast<unsigned char>(ascii_to_lower(s1[i]))) -
                     static_cast<int>(static_cast<unsigned char>(ascii_to_lower(s2[i])));
        if (delta != 0) {
            return delta;
        }
    }

    return 0;
}

} // namespace detail

template<typename Iterator>
void join(Iterator first, Iterator last, std::string_view sep, std::string& out) {
    detail::join_impl(first, last, sep, out);
}

template<typename Iterator>
std::string join(Iterator first, Iterator last, std::string_view sep) {
    std::string out;
    join(first, last, sep, out);
    return out;
}

template<typename Container>
void join(const Container& c, std::string_view sep, std::string& out) {
    using std::begin;
    using std::end;
    join(begin(c), end(c), sep, out);
}

template<typename Container>
std::string join(const Container& c, std::string_view sep) {
    std::string out;
    join(c, sep, out);
    return out;
}

template<typename T>
void join(std::initializer_list<T> il, std::string_view sep, std::string& out) {
    join(il.begin(), il.end(), sep, out);
}

template<typename T>
std::string join(std::initializer_list<T> il, std::string_view sep) {
    std::string out;
    join(il, sep, out);
    return out;
}

// The signature of the function object `Appender` should be equivalent to
//   void fn(const Type& entry, std::string& out);
// where the `Type` is element entry's type.

template<typename Iterator, typename Appender>
void join(Iterator first, Iterator last, std::string_view sep, std::string& out, Appender&& ap) {
    out.clear();
    if (first == last) {
        return;
    }

    detail::join_append(first, last, sep, out, std::forward<Appender>(ap));
}

template<typename Iterator, typename Appender>
std::string join(Iterator first, Iterator last, std::string_view sep, Appender&& ap) {
    std::string out;
    join(first, last, sep, std::forward<Appender>(ap), out);
    return out;
}

template<typename Container, typename Appender>
void join(const Container& c, std::string_view sep, std::string& out, Appender&& ap) {
    using std::begin;
    using std::end;
    join(begin(c), end(c), sep, out, std::forward<Appender>(ap));
}

template<typename Container, typename Appender>
std::string join(const Container& c, std::string_view sep, Appender&& ap) {
    std::string out;
    join(c, sep, out, std::forward<Appender>(ap));
    return out;
}

template<typename T, typename Appender>
void join(std::initializer_list<T> il, std::string_view sep, std::string& out, Appender&& ap) {
    join(il.begin(), il.end(), sep, out, std::forward<Appender>(ap));
}

template<typename T, typename Appender>
std::string join(std::initializer_list<T> il, std::string_view sep, Appender&& ap) {
    std::string out;
    join(il, sep, out, std::forward<Appender>(ap));
    return out;
}

constexpr bool equals_ignore_ascii_case(std::string_view s1, std::string_view s2) noexcept {
    return s1.size() == s2.size() && detail::compare_n_ignore_ascii_case(s1, s2, s1.size()) == 0;
}

constexpr bool starts_with(std::string_view str, std::string_view prefix) noexcept {
    return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
}

constexpr bool ends_with(std::string_view str, std::string_view suffix) noexcept {
    return str.size() >= suffix.size() &&
           str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

constexpr bool starts_with_ignore_ascii_case(std::string_view str,
                                             std::string_view prefix) noexcept {
    return str.size() >= prefix.size() &&
           equals_ignore_ascii_case(str.substr(0, prefix.size()), prefix);
}

constexpr bool ends_with_ignore_ascii_case(std::string_view str,
                                           std::string_view suffix) noexcept {
    return str.size() >= suffix.size() &&
           equals_ignore_ascii_case(str.substr(str.size() - suffix.size()), suffix);
}

// The behavior is undefined if given `delim` is empty.
class by_string {
public:
    explicit by_string(std::string delim)
        : delimiter_(std::move(delim)) {
        assert(!delimiter_.empty());
    }

    std::size_t find(std::string_view text, std::size_t pos) const noexcept {
        return delimiter_.size() == 1 ? text.find(delimiter_[0], pos)
                                      : text.find(delimiter_, pos);
    }

    std::size_t size() const noexcept {
        return delimiter_.size();
    }

private:
    std::string delimiter_;
};

class by_char {
public:
    explicit by_char(char ch) noexcept
        : ch_(ch) {}

    std::size_t find(std::string_view text, std::size_t pos) const noexcept {
        return text.find(ch_, pos);
    }

    std::size_t size() const noexcept { // NOLINT(readability-convert-member-functions-to-static)
        return 1;
    }

private:
    char ch_;
};

// The behavior is undefined if given `delim` is empty.
class by_any_char {
public:
    explicit by_any_char(std::string delims)
        : delimiters_(std::move(delims)) {
        assert(!delimiters_.empty());
    }

    std::size_t find(std::string_view text, std::size_t pos) const noexcept {
        return delimiters_.size() == 1 ? text.find_first_of(delimiters_[0], pos)
                                       : text.find_first_of(delimiters_, pos);
    }

    std::size_t size() const noexcept { // NOLINT(readability-convert-member-functions-to-static)
        return 1;
    }

private:
    std::string delimiters_;
};

struct allow_any {
    bool operator()(std::string_view) const noexcept {
        return true;
    }
};

struct skip_empty {
    bool operator()(std::string_view str) const noexcept {
        return !str.empty();
    }
};

namespace detail {

template<typename Delimiter>
struct select_delimiter {
    using type = Delimiter;
};

template<>
struct select_delimiter<char> {
    using type = by_char;
};

template<>
struct select_delimiter<char*> {
    using type = by_string;
};

template<>
struct select_delimiter<const char*> {
    using type = by_string;
};

template<>
struct select_delimiter<std::string_view> {
    using type = by_string;
};

template<>
struct select_delimiter<std::string> {
    using type = by_string;
};

} // namespace detail

template<typename Delimiter>
auto split(std::string_view text, Delimiter delim) {
    using delimiter_t = typename detail::select_delimiter<Delimiter>::type;
    return detail::split_view<std::string_view, delimiter_t, allow_any>(
            text, delimiter_t(delim), allow_any{});
}

template<typename Delimiter, typename Predicate>
auto split(std::string_view text, Delimiter delim, Predicate predicate) {
    using delimiter_t = typename detail::select_delimiter<Delimiter>::type;
    return detail::split_view<std::string_view, delimiter_t, Predicate>(
            text, delimiter_t(delim), predicate);
}

// Following two functions will be selected if and only if the `text` is a rvalue
// `std::string`; using a direct cast instead of a `std::move()` or `std::forward()`
// call to imply this semantic behavior while avoiding potential lint issues.

template<typename Delimiter,
         typename StringType,
         std::enable_if_t<std::is_same_v<std::remove_cv_t<StringType>, std::string>, int> = 0>
auto split(StringType&& text, Delimiter delim) {
    using delimiter_t = typename detail::select_delimiter<Delimiter>::type;
    return detail::split_view<std::string, delimiter_t, allow_any>(
            static_cast<std::string&&>(text), delimiter_t(delim), allow_any{});
}

template<typename Delimiter,
         typename StringType,
         typename Predicate,
         std::enable_if_t<std::is_same_v<std::remove_cv_t<StringType>, std::string>, int> = 0>
auto split(StringType&& text, Delimiter delim, Predicate predicate) {
    using delimiter_t = typename detail::select_delimiter<Delimiter>::type;
    return detail::split_view<std::string, delimiter_t, Predicate>(
            static_cast<std::string&&>(text), delimiter_t(delim), predicate);
}

} // namespace esl::strings

#endif // ESL_STRINGS_H_
