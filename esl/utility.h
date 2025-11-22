// Copyright (c) 2025 Kingsley Chen <kingsamchen at gmail dot com>
// This file is subject to the terms of license that can be found
// in the LICENSE file.

#pragma once

#include <type_traits>

namespace esl {

template<typename E>
constexpr std::underlying_type_t<E> to_underlying(E e) {
    return static_cast<std::underlying_type_t<E>>(e);
}

template<typename... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};

template<typename... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

} // namespace esl
