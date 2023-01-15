// Copyright (c) 2023 Kingsley Chen <kingsamchen at gmail dot com>
// This file is subject to the terms of license that can be found
// in the LICENSE file.

#pragma once

#ifndef ESL_DETAIL_SECURE_CRT_H_
#define ESL_DETAIL_SECURE_CRT_H_

#include <cstdio>
#include <string>

namespace esl::detail {

inline std::FILE* fopen(const std::string& filepath, const char* mode) {
#if defined(_WIN32)
    std::FILE* fp{nullptr};
    // The global errno is set on error anyway.
    (void)::fopen_s(&fp, filepath.c_str(), mode);
    return fp;
#else
    return std::fopen(filepath.c_str(), mode);
#endif
}

} // namespace esl::detail

#endif // ESL_DETAIL_SECURE_CRT_H_
