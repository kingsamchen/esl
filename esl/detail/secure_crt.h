// Copyright (c) 2023 Kingsley Chen <kingsamchen at gmail dot com>
// This file is subject to the terms of license that can be found
// in the LICENSE file.

#pragma once

#include <cstdio>
#include <string>

#include "esl/unique_handle.h"

#if defined(_WIN32)
#include "esl/ignore_unused.h"
#endif

namespace esl::detail {

inline unique_file fopen(const std::string& filepath, const char* mode) {
#if defined(_WIN32)
    std::FILE* fp{nullptr};
    // The global errno is set on error anyway.
    ignore_unused(::fopen_s(&fp, filepath.c_str(), mode));
    return wrap_unique_file(fp);
#else
    return wrap_unique_file(std::fopen(filepath.c_str(), mode));
#endif
}

} // namespace esl::detail
