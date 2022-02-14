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

#include "esl/detail/strings_split.h"

namespace esl::strings {
namespace detail {

template<typename T>
struct is_sizable_str : std::false_type {};

template<>
struct is_sizable_str<std::string> : std::true_type {};

template<>
struct is_sizable_str<std::string_view> : std::true_type {};

template<typename Iterator, typename = void>
struct is_sizable_str_multipass_range : std::false_type {};

template<typename Iterator>
struct is_sizable_str_multipass_range<
        Iterator,
        std::enable_if_t<
                is_sizable_str<typename std::iterator_traits<Iterator>::value_type>::value &&
                std::is_convertible_v<typename std::iterator_traits<Iterator>::iterator_category,
                                      std::forward_iterator_tag>>> : std::true_type {};

// The function presumes the range is not empty.
template<typename Iterator, typename Appender>
void join_append(Iterator first, Iterator last, std::string_view sep, std::string& out,
                 Appender&& appender) {
    assert(first != last);
    appender(*first, out);
    for (auto it = ++first; it != last; ++it) {
        out.append(sep);
        appender(*it, out);
    }
}

template<typename T>
std::enable_if_t<is_sizable_str<T>::value> to_append(const T& value, std::string& out) {
    out.append(value.data(), value.size());
}

template<typename T>
std::enable_if_t<std::is_convertible_v<T, const char*>> to_append(T value, std::string& out) {
    out.append(value);
}

template<typename T>
std::enable_if_t<std::is_same_v<std::remove_const_t<T>, char>>
to_append(T value, std::string& out) {
    out.append(1, value);
}

template<typename Iterator>
std::enable_if_t<is_sizable_str_multipass_range<Iterator>::value>
join_impl(Iterator first, Iterator last, std::string_view sep, std::string& out) {
    out.clear();
    if (first == last) {
        return;
    }

    std::size_t total_len = first->size();
    for (auto it = std::next(first); it != last; ++it) {
        total_len += sep.size() + it->size();
    }

    out.reserve(total_len);

    join_append(first, last, sep, out, [](const auto& value, std::string& os) {
        to_append(value, os);
    });
}

template<typename Iterator>
std::enable_if_t<!is_sizable_str_multipass_range<Iterator>::value>
join_impl(Iterator first, Iterator last, std::string_view sep, std::string& out) {
    out.clear();
    if (first == last) {
        return;
    }

    join_append(first, last, sep, out, [](const auto& value, std::string& os) {
        to_append(value, os);
    });
}

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

class by_string {
public:
    explicit by_string(std::string delim)
        : delimiter_(std::move(delim)) {}

    std::size_t find(std::string_view text, std::size_t pos) const noexcept {
        return text.find(delimiter_, pos);
    }

    std::size_t size() const noexcept {
        return delimiter_.size();
    }

private:
    std::string delimiter_;
};

class by_any_char {
public:
    explicit by_any_char(std::string delims)
        : delimiters_(std::move(delims)) {}

    std::size_t find(std::string_view text, std::size_t pos) const noexcept {
        return text.find_first_of(delimiters_, pos);
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

} // namespace esl::strings

#endif // ESL_STRINGS_H_
