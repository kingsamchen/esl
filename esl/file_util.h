// Copyright (c) 2023 Kingsley Chen <kingsamchen at gmail dot com>
// This file is subject to the terms of license that can be found
// in the LICENSE file.

#pragma once

#ifndef ESL_FILE_UTIL_H_
#define ESL_FILE_UTIL_H_

#include <cerrno>
#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <string>
#include <string_view>
#include <system_error>

#include "esl/detail/files.h"
#include "esl/detail/secure_crt.h"
#include "esl/scope_guard.h"

namespace esl {

// `content` may contain partially read data on error.
inline void read_file_to_string(const std::string& path,
                                std::string& content,
                                std::error_code& ec) {
    content.clear();
    ec.clear();

    FILE* fp = detail::fopen(path, "rb");
    if (!fp) {
        ec.assign(errno, std::generic_category());
        return;
    }

    ESL_ON_SCOPE_EXIT {
        std::fclose(fp);
    };

    constexpr auto initial_chunk_size = static_cast<std::size_t>(1024) * 4;
    constexpr auto default_chunk_size = static_cast<std::size_t>(1024) * 16;

    // Once we get the file size, read one more byte to ensure reach of EOF, which can be detected
    // by `feof()`.
    // If the file is a special file like one of files under /proc, or an error occurred during
    // the function call, the `file_size` is 0; then we use the smaller `initial_chunk_size` as the
    // probe size in first read, to reduce possible memory overhead of the `content` resizing, but
    // subsequent reads will reset to using `default_chunk_size` to speed up reading.
    const std::size_t file_size = detail::get_file_size(fp);
    auto read_chunk_size = file_size > 0 ? file_size + 1 : initial_chunk_size;
    content.resize(read_chunk_size, '\0');
    std::size_t total_size_read{0};
    while (true) {
        auto size_read = std::fread(content.data() + total_size_read, 1, read_chunk_size, fp);
        total_size_read += size_read;
        if (size_read != read_chunk_size) {
            if (ferror(fp)) {
                ec.assign(EIO, std::generic_category());
                break;
            }

            if (feof(fp)) {
                break;
            }
        }

        if (read_chunk_size == initial_chunk_size) {
            read_chunk_size = default_chunk_size;
        }

        content.resize(total_size_read + read_chunk_size);
    }

    content.resize(total_size_read);
}

// Throws `std::filesystem::filesystem_error` on error.
inline void read_file_to_string(const std::string& path, std::string& content) {
    std::error_code ec;
    read_file_to_string(path, content, ec);
    if (ec) {
        throw std::filesystem::filesystem_error("read file error", path, ec);
    }
}

// If file already exists, will overwrite the file.
inline void write_to_file(const std::string& path, std::string_view content, std::error_code& ec) {
    ec.clear();

    std::FILE* fp = detail::fopen(path, "wb");
    if (!fp) {
        ec.assign(errno, std::generic_category());
        return;
    }

    ESL_ON_SCOPE_EXIT {
        std::fclose(fp);
    };

    auto size_written = std::fwrite(content.data(), 1, content.size(), fp);
    if (size_written != content.size() && ferror(fp)) {
        ec.assign(EIO, std::generic_category());
    }
}

// Throws `std::filesystem::filesystem_error` on error.
inline void write_to_file(const std::string& path, std::string_view content) {
    std::error_code ec;
    write_to_file(path, content, ec);
    if (ec) {
        throw std::filesystem::filesystem_error("write file error", path, ec);
    }
}

} // namespace esl

#endif // ESL_FILE_UTIL_H_
