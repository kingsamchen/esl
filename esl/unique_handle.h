// Copyright (c) 2021 Kingsley Chen <kingsamchen at gmail dot com>
// This file is subject to the terms of license that can be found
// in the LICENSE file.

#pragma once

#ifndef ESL_UNIQUE_HANDLE_H_
#define ESL_UNIQUE_HANDLE_H_

#include <memory>

#if defined(_WIN32)
#include <Windows.h>
#else
#include <unistd.h>
#endif

namespace esl {

template<typename Traits>
class handle_ptr {
public:
    using handle_type = typename Traits::handle_type;

    handle_ptr() noexcept = default;

    handle_ptr(std::nullptr_t) noexcept {} // NOLINT(google-explicit-constructor)

    handle_ptr(handle_type handle) noexcept // NOLINT(google-explicit-constructor)
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

    void operator()(pointer ptr) {
        Traits::close(ptr);
    }
};

#if defined(_WIN32)

struct win_handle_traits {
    using handle_type = HANDLE;

    static bool is_valid(handle_type handle) noexcept {
        return handle != null_handle;
    }

    static void close(handle_type handle) noexcept {
        ::CloseHandle(handle);
    }

    static constexpr handle_type null_handle{nullptr};
};

using win_handle_deleter = handle_ptr_deleter<win_handle_traits>;
using unique_win_handle = std::unique_ptr<win_handle_traits::handle_type, win_handle_deleter>;

inline unique_win_handle wrap_unique_win_handle(HANDLE raw_handle) {
    return unique_win_handle(raw_handle);
}

struct winfile_handle_traits {
    using handle_type = HANDLE;

    static bool is_valid(handle_type handle) noexcept {
        return handle != null_handle;
    }

    static void close(handle_type handle) noexcept {
        ::CloseHandle(handle);
    }

    static constexpr handle_type null_handle{INVALID_HANDLE_VALUE};
};

using winfile_handle_deleter = handle_ptr_deleter<winfile_handle_traits>;
using unique_winfile_handle =
        std::unique_ptr<winfile_handle_traits::handle_type, winfile_handle_deleter>;

inline unique_winfile_handle wrap_unique_winfile_handle(HANDLE raw_file_handle) {
    return unique_winfile_handle(raw_file_handle);
}

#else

struct fd_traits {
    using handle_type = int;

    static bool is_valid(handle_type handle) noexcept {
        return handle != null_handle;
    }

    static void close(handle_type handle) noexcept {
        ::close(handle);
    }

    static constexpr handle_type null_handle{-1};
};

using fd_deleter = handle_ptr_deleter<fd_traits>;
using unique_fd = std::unique_ptr<fd_traits::handle_type, fd_deleter>;

inline unique_fd wrap_unique_fd(int raw_fd) {
    return unique_fd(raw_fd);
}

#endif

} // namespace esl

#endif // ESL_UNIQUE_HANDLE_H_
