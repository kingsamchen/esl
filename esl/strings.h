// Copyright (c) 2023 Kingsley Chen <kingsamchen at gmail dot com>
// This file is subject to the terms of license that can be found
// in the LICENSE file.

#pragma once

#include <cassert>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

#include "esl/detail/strings_join.h"
#include "esl/detail/strings_match.h"
#include "esl/detail/strings_split.h"

namespace esl::strings {

//
// match
//

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

//
// join
//

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

//
// split
//

// The behavior is undefined if given `delim` is empty.
class by_string {
public:
    explicit by_string(std::string delim)
        : delimiter_(std::move(delim)) {
        assert(!delimiter_.empty());
    }

    template<typename StringViewLike,
             std::enable_if_t<std::is_same_v<StringViewLike, std::string_view>, int> = 0>
    explicit by_string(StringViewLike delim)
        : by_string(std::string{delim}) {}

    [[nodiscard]] std::size_t find(std::string_view text, std::size_t pos) const noexcept {
        return delimiter_.size() == 1 ? text.find(delimiter_[0], pos)
                                      : text.find(delimiter_, pos);
    }

    [[nodiscard]] std::size_t size() const noexcept {
        return delimiter_.size();
    }

private:
    std::string delimiter_;
};

class by_char {
public:
    explicit by_char(char ch) noexcept
        : ch_(ch) {}

    [[nodiscard]] std::size_t find(std::string_view text, std::size_t pos) const noexcept {
        return text.find(ch_, pos);
    }

    static std::size_t size() noexcept {
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

    template<typename StringViewLike,
             std::enable_if_t<std::is_same_v<StringViewLike, std::string_view>, int> = 0>
    explicit by_any_char(StringViewLike delim)
        : by_any_char(std::string{delim}) {}

    [[nodiscard]] std::size_t find(std::string_view text, std::size_t pos) const noexcept {
        return delimiters_.size() == 1 ? text.find_first_of(delimiters_[0], pos)
                                       : text.find_first_of(delimiters_, pos);
    }

    static std::size_t size() noexcept {
        return 1;
    }

private:
    std::string delimiters_;
};

// The behavior is undefined if given `len` is 0.
class by_length {
public:
    explicit by_length(std::size_t len)
        : limit_len_(len) {
        assert(len > 0);
    }

    [[nodiscard]] std::size_t find(std::string_view text, std::size_t pos) const noexcept {
        const auto next_pos = pos + limit_len_;
        return next_pos < text.size() ? next_pos : std::string_view::npos;
    }

    static std::size_t size() noexcept {
        return 0;
    }

private:
    std::size_t limit_len_;
};

struct allow_any {
    bool operator()(std::string_view /*unused*/) const noexcept {
        return true;
    }
};

struct skip_empty {
    bool operator()(std::string_view str) const noexcept {
        return !str.empty();
    }
};

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
auto split(StringType&& text, // NOLINT(cppcoreguidelines-missing-std-forward)
           Delimiter delim) {
    using delimiter_t = typename detail::select_delimiter<Delimiter>::type;
    return detail::split_view<std::string, delimiter_t, allow_any>(
            static_cast<std::string&&>(text), delimiter_t(delim), allow_any{});
}

template<typename Delimiter,
         typename StringType,
         typename Predicate,
         std::enable_if_t<std::is_same_v<std::remove_cv_t<StringType>, std::string>, int> = 0>
auto split(StringType&& text, // NOLINT(cppcoreguidelines-missing-std-forward)
           Delimiter delim,
           Predicate predicate) {
    using delimiter_t = typename detail::select_delimiter<Delimiter>::type;
    return detail::split_view<std::string, delimiter_t, Predicate>(
            static_cast<std::string&&>(text), delimiter_t(delim), predicate);
}

//
// trim
//

[[nodiscard]] constexpr std::string_view trim_prefix(std::string_view str,
                                                     std::string_view prefix) noexcept {
    if (starts_with(str, prefix)) {
        str.remove_prefix(prefix.size());
    }

    return str;
}

inline void trim_prefix_inplace(std::string& str, std::string_view prefix) {
    if (starts_with(str, prefix)) {
        str.erase(0, prefix.size());
    }
}

[[nodiscard]] constexpr std::string_view trim_suffix(std::string_view str,
                                                     std::string_view suffix) noexcept {
    if (ends_with(str, suffix)) {
        str.remove_suffix(suffix.size());
    }

    return str;
}

inline void trim_suffix_inplace(std::string& str, std::string_view suffix) {
    if (ends_with(str, suffix)) {
        str.erase(str.size() - suffix.size());
    }
}

[[nodiscard]] constexpr std::string_view trim_left(std::string_view str,
                                                   std::string_view chars) noexcept {
    auto pos = str.find_first_not_of(chars);
    return pos == std::string_view::npos ? std::string_view{} : str.substr(pos);
}

inline void trim_left_inplace(std::string& str, std::string_view chars) {
    auto pos = str.find_first_not_of(chars);
    str.erase(0, pos);
}

[[nodiscard]] constexpr std::string_view trim_right(std::string_view str,
                                                    std::string_view chars) noexcept {
    auto pos = str.find_last_not_of(chars);
    return pos == std::string_view::npos ? std::string_view{} : str.substr(0, pos + 1);
}

inline void trim_right_inplace(std::string& str, std::string_view chars) {
    auto pos = str.find_last_not_of(chars);
    if (auto start = pos + 1; start != str.size()) {
        str.erase(start);
    }
}

[[nodiscard]] constexpr std::string_view trim(std::string_view str,
                                              std::string_view chars) noexcept {
    return trim_right(trim_left(str, chars), chars);
}

inline void trim_inplace(std::string& str, std::string_view chars) {
    trim_right_inplace(str, chars);
    trim_left_inplace(str, chars);
}

} // namespace esl::strings
