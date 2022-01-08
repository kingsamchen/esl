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

namespace esl {
namespace strings {
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

} // namespace strings
} // namespace esl

#endif // ESL_STRINGS_H_
