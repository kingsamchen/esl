// Copyright (c) 2024 Kingsley Chen <kingsamchen at gmail dot com>
// This file is subject to the terms of license that can be found
// in the LICENSE file.

#pragma once

#include "esl/macros.h"

namespace esl {

// TODO(Kingsley): Remove NOLINT once MSVC has upgraded its shipped clang-tidy.
// See https://github.com/llvm/llvm-project/issues/87697
template<typename... Ts>
ESL_FORCEINLINE constexpr void
ignore_unused(Ts&&... /*unused_vars*/) {} // NOLINT(cppcoreguidelines-missing-std-forward)

template<typename... Ts>
ESL_FORCEINLINE constexpr void ignore_unused() {}

} // namespace esl
