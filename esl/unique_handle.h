// Copyright (c) 2023 Kingsley Chen <kingsamchen at gmail dot com>
// This file is subject to the terms of license that can be found
// in the LICENSE file.

#pragma once

#include <cstdio>
#include <memory>

#if defined(_WIN32)
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include "esl/ignore_unused.h"

namespace esl {

template<typename Traits>
class handle_ptr {
public:
    using handle_type = typename Traits::handle_type;

    handle_ptr() noexcept = default;

    handle_ptr(std::nullptr_t) noexcept {} // NOLINT(google-explicit-constructor)

    explicit handle_ptr(handle_type handle) noexcept
        : handle_(handle) {}

    explicit operator bool() const noexcept {
        return Traits::is_valid(handle_);
    }

    operator handle_type() const noexcept { // NOLINT(google-explicit-constructor)
        return handle_;
    }

    friend bool operator==(handle_ptr lhs, handle_ptr rhs) noexcept {
        return lhs.handle_ == rhs.handle_;
    }

    friend bool operator!=(handle_ptr lhs, handle_ptr rhs) noexcept {
        return !(lhs == rhs);
    }

private:
    handle_type handle_{Traits::null_handle};
};

template<typename Traits>
struct handle_ptr_deleter {
    using pointer = handle_ptr<Traits>;

    void operator()(pointer ptr) const {
        Traits::close(ptr);
    }
};

#if defined(_WIN32)

//
// Windows HANDLE
//

struct win_handle_traits {
    using handle_type = HANDLE;

    static bool is_valid(handle_type handle) noexcept {
        return handle != null_handle;
    }

    static void close(handle_type handle) noexcept {
        ignore_unused(::CloseHandle(handle));
    }

    static constexpr handle_type null_handle{nullptr};
};

using win_handle_deleter = handle_ptr_deleter<win_handle_traits>;
using unique_win_handle = std::unique_ptr<win_handle_deleter::pointer, win_handle_deleter>;

inline unique_win_handle wrap_unique_win_handle(HANDLE raw_handle) {
    return unique_win_handle(unique_win_handle::pointer{raw_handle});
}

//
// Windows HANDLE for file
//

struct winfile_handle_traits {
    using handle_type = HANDLE;

    static bool is_valid(handle_type handle) noexcept {
        return handle != null_handle;
    }

    static void close(handle_type handle) noexcept {
        ignore_unused(::CloseHandle(handle));
    }

    // TODO(KC): use std::bit_cast when upgrading to C++20
    inline static const handle_type null_handle{INVALID_HANDLE_VALUE};
};

using winfile_handle_deleter = handle_ptr_deleter<winfile_handle_traits>;
using unique_winfile_handle =
        std::unique_ptr<winfile_handle_deleter::pointer, winfile_handle_deleter>;

inline unique_winfile_handle wrap_unique_winfile_handle(HANDLE raw_file_handle) {
    return unique_winfile_handle(unique_winfile_handle::pointer{raw_file_handle});
}

#else

//
// fd
//

struct fd_traits {
    using handle_type = int;

    static bool is_valid(handle_type handle) noexcept {
        return handle != null_handle;
    }

    static void close(handle_type handle) noexcept {
        ignore_unused(::close(handle));
    }

    static constexpr handle_type null_handle{-1};
};

using fd_deleter = handle_ptr_deleter<fd_traits>;
using unique_fd = std::unique_ptr<fd_deleter::pointer, fd_deleter>;

inline unique_fd wrap_unique_fd(int raw_fd) {
    return unique_fd(unique_fd::pointer{raw_fd});
}

#endif

//
// FILE
//

struct file_deleter {
    void operator()(std::FILE* fp) const noexcept {
        ignore_unused(std::fclose(fp));
    }
};

using unique_file = std::unique_ptr<std::FILE, file_deleter>;

inline unique_file wrap_unique_file(std::FILE* fp) {
    return unique_file(fp);
}

} // namespace esl
