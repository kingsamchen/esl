// Copyright (c) 2021 Kingsley Chen <kingsamchen at gmail dot com>
// This file is subject to the terms of license that can be found
// in the LICENSE file.

#include <tuple>
#include <type_traits>

#if !defined(_WIN32)
#include <fcntl.h>
#endif

#include "doctest/doctest.h"

#include "esl/unique_handle.h"
#include "tests/test_util.h"

namespace {

struct fake_handle_traits {
    using handle_type = int;

    static bool is_valid(handle_type handle) noexcept {
        return handle != null_handle;
    }

    static void close(handle_type) noexcept {
        // do nothing
    }

    static constexpr handle_type null_handle{-1};
};

template<typename T, typename = void>
struct is_equality_comparable : std::false_type {};

template<typename T>
struct is_equality_comparable<
        T,
        std::void_t<
                std::enable_if_t<
                        std::conjunction_v<
                                std::is_same<bool, decltype(T{} == T{})>,
                                std::is_same<bool, decltype(T{} != T{})>>>>> : std::true_type {};

template<typename T>
inline constexpr bool is_equality_comparable_v = is_equality_comparable<T>::value;

std::FILE* safe_fopen(const char* path, const char* mode) {
#if defined(_WIN32)
    std::FILE* fp{};
    std::ignore = ::fopen_s(&fp, path, mode);
    return fp;
#else
    return std::fopen(path, mode);
#endif
}

TEST_SUITE_BEGIN("unique_handle");

TEST_CASE("handle_ptr must meet nullability expression requirements") {
    using fake_handle_ptr = esl::handle_ptr<fake_handle_traits>;

    SUBCASE("satisfy type traits") {
        CHECK(is_equality_comparable_v<fake_handle_ptr>);
        CHECK(std::is_default_constructible_v<fake_handle_ptr>);
        CHECK(std::is_copy_constructible_v<fake_handle_ptr>);
        CHECK(std::is_copy_assignable_v<fake_handle_ptr>);
        CHECK(std::is_destructible_v<fake_handle_ptr>);
    }

    SUBCASE("value initialized handle_ptr must produce a null value of that type") {
        fake_handle_ptr ptr{};
        // Relies on implicit conversion.
        CHECK(ptr == fake_handle_traits::null_handle);
    }

    SUBCASE("must be contextually convertible to bool") {
        {
            const fake_handle_ptr ptr1{};
            bool valid = static_cast<bool>(ptr1);
            CHECK_FALSE(valid);
        }

        {
            const fake_handle_ptr ptr2{1};
            bool valid = static_cast<bool>(ptr2);
            CHECK(valid);
        }
    }

    SUBCASE("must satisfy following additional expressions") {
        SUBCASE("construction from nullptr") {
            fake_handle_ptr ptr1(nullptr);
            CHECK(ptr1 == nullptr);

            fake_handle_ptr ptr2 = nullptr;
            CHECK(ptr2 == nullptr);
        }

        SUBCASE("a temporary from nullptr is equivalent to nullptr") {
            CHECK(fake_handle_ptr(nullptr) == nullptr);
        }

        SUBCASE("compare with nullptr") {
            fake_handle_ptr np_ptr(nullptr);
            CHECK(np_ptr == nullptr);
            CHECK(nullptr == np_ptr);
            CHECK_FALSE(np_ptr != nullptr);
            CHECK_FALSE(nullptr != np_ptr);

            fake_handle_ptr ptr(1);
            CHECK_FALSE(ptr == nullptr);
            CHECK_FALSE(nullptr == ptr);
            CHECK(ptr != nullptr);
            CHECK(nullptr != ptr);
        }

        SUBCASE("compare with other handle_ptr") {
            CHECK(fake_handle_ptr(nullptr) == fake_handle_ptr(nullptr));
            CHECK(fake_handle_ptr(nullptr) != fake_handle_ptr(1));
        }

        SUBCASE("equivalent to nullptr after assigned to nullptr") {
            fake_handle_ptr ptr(1);
            CHECK(ptr != nullptr);
            ptr = nullptr;
            CHECK(ptr == nullptr);
        }
    }

    SUBCASE("value initialized handle_ptr is equivalent to nullptr") {
        CHECK(fake_handle_ptr{} == nullptr);
        CHECK_FALSE(fake_handle_ptr{} != nullptr);
    }
}

#if defined(_WIN32)

TEST_CASE("unique_win_handle should work as expected") {
    using esl::unique_win_handle;
    using esl::wrap_unique_win_handle;

    SUBCASE("default initialized should be equivalent to nullptr") {
        unique_win_handle handle;
        CHECK(handle == nullptr);
    }

    SUBCASE("INVALID_HANDLE_VALUE is a valid value for unique_win_handle") {
        auto handle = wrap_unique_win_handle(INVALID_HANDLE_VALUE);
        CHECK(handle != nullptr);
    }

    SUBCASE("non-empty handle") {
        static constexpr wchar_t event_name[] = L"test_event";

        auto handle = wrap_unique_win_handle(::CreateEventW(nullptr, TRUE, TRUE, event_name));
        REQUIRE(handle != nullptr);
        REQUIRE(handle.get() != nullptr);
        auto sync_handle = wrap_unique_win_handle(::OpenEventW(SYNCHRONIZE, FALSE, event_name));
        REQUIRE(sync_handle != nullptr);
        sync_handle.reset();
        REQUIRE(sync_handle == nullptr);

        handle.reset();
        CHECK(handle == nullptr);
        CHECK_EQ(::OpenEventW(SYNCHRONIZE, FALSE, event_name), nullptr);
    }
}

TEST_CASE("unique_winfile_handle should satisfy") {
    using esl::unique_winfile_handle;
    using esl::wrap_unique_winfile_handle;

    SUBCASE("default initialized should be") {
        unique_winfile_handle handle;

        SUBCASE("equivalent to nullptr") {
            CHECK(handle == nullptr);
        }

        SUBCASE("handle value is equal to INVALID_HANDLE_VALUE but not NULL") {
            // Please differentiate NULL value from nullptr.
            CHECK(handle.get() == INVALID_HANDLE_VALUE);
            CHECK(static_cast<HANDLE>(handle.get()) != NULL);
        }
    }

    SUBCASE("0 handle value is a valid value for unique_winfile_handle") {
        auto handle = wrap_unique_winfile_handle(nullptr);
        CHECK(handle != nullptr);
    }
}

#else

TEST_CASE("unique_fd should satisfy") {
    using esl::unique_fd;
    using esl::wrap_unique_fd;

    SUBCASE("default initialized should be") {
        unique_fd fd_handle;

        SUBCASE("equivalent to nullptr") {
            CHECK(fd_handle == nullptr);
        }

        SUBCASE("fd value is -1") {
            CHECK(fd_handle.get() == -1);
        }
    }

    SUBCASE("simple use cases") {
        auto fd = wrap_unique_fd(open("/proc/cpuinfo", O_RDONLY));
        REQUIRE(fd != nullptr);
        REQUIRE(fd.get() != -1);

        fd.reset();
        CHECK(fd == nullptr);
        CHECK(fd.get() == -1);
    }
}

#endif

TEST_CASE("unique_file as FILE* wrapper") {
    using esl::unique_file;
    static_assert(std::is_same_v<unique_file::pointer, std::FILE*>);
    static_assert(std::is_same_v<unique_file::element_type, std::FILE>);
    static_assert(std::is_same_v<unique_file::deleter_type, esl::file_deleter>);

    SUBCASE("simple use case") {
        unique_file file;
        REQUIRE(file == nullptr);
        auto raw_fp = safe_fopen(tests::new_test_filepath().c_str(), "w");
        INFO("create file for writing failed; errno=", errno);
        REQUIRE(raw_fp != nullptr);
        file = esl::wrap_unique_file(raw_fp);
        CHECK(file != nullptr);
        const std::string str{"this is a test text"};
        auto size_written = std::fwrite(str.data(), 1, str.size(), file.get());
        CHECK_EQ(size_written, str.size());

        // Now reset the unique file.
        file = nullptr;
        CHECK(file == nullptr);
        CHECK_EQ(file.get(), nullptr);
    }
}

TEST_SUITE_END();

} // namespace
