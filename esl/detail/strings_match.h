// Copyright (c) 2023 Kingsley Chen <kingsamchen at gmail dot com>
// This file is subject to the terms of license that can be found
// in the LICENSE file.

#pragma once

#include <algorithm>
#include <cassert>
#include <string_view>

namespace esl::strings::detail {

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

} // namespace esl::strings::detail
