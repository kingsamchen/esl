// Copyright (c) 2023 Kingsley Chen <kingsamchen at gmail dot com>
// This file is subject to the terms of license that can be found
// in the LICENSE file.

#pragma once

#include <cassert>
#include <cstddef>
#include <iterator>
#include <string>
#include <string_view>
#include <type_traits>

namespace esl::strings::detail {

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
    auto&& ap = std::forward<Appender>(appender);
    ap(*first, out);
    for (auto it = ++first; it != last; ++it) {
        out.append(sep);
        ap(*it, out);
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

} // namespace esl::strings::detail
