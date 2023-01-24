// Copyright (c) 2023 Kingsley Chen <kingsamchen at gmail dot com>
// This file is subject to the terms of license that can be found
// in the LICENSE file.

#include <filesystem>
#include <fstream>
#include <string>
#include <system_error>

#include "doctest/doctest.h"

#include "esl/file_util.h"
#include "esl/strings.h"
#include "tests/test_util.h"

namespace fs = std::filesystem;

using namespace std::string_view_literals;

namespace {

TEST_SUITE_BEGIN("file_util");

TEST_CASE("write file then read") {
    auto file = tests::new_test_filepath();
    CAPTURE(file);
    const std::string str{"this is a test text\n"};
    esl::write_to_file(file, str);
    std::string read_content;
    esl::read_file_to_string(file, read_content);
    CHECK_EQ(read_content, str);
}

TEST_CASE("overwrite if file already exists") {
    auto file = tests::new_test_filepath();
    CAPTURE(file);

    std::string read_content;
    constexpr auto s1 = "this is a test text"sv;
    esl::write_to_file(file, s1);
    esl::read_file_to_string(file, read_content);
    CHECK_EQ(read_content, s1);

    // Overwrite the file content.
    constexpr auto s2 = "a quick fox jumps over a lazy dog"sv;
    REQUIRE_NE(s2, s1);
    esl::write_to_file(file, s2);
    esl::read_file_to_string(file, read_content);
    CHECK_EQ(read_content, s2);
}

TEST_CASE("write to the file don't have write permission") {
    auto file = tests::new_test_filepath();
    CAPTURE(file);

    // Create the file first.
    std::ofstream out(file);
    out.close();

    // Remove owner write permission.
    // NOTE: must remove all three write permissions on Windows.
    REQUIRE((fs::status(file).permissions() & fs::perms::owner_write) != fs::perms::none);
    auto write_perms = fs::perms::owner_write | fs::perms::others_write | fs::perms::group_write;
    fs::permissions(file, write_perms, fs::perm_options::remove);
    REQUIRE((fs::status(file).permissions() & fs::perms::owner_write) == fs::perms::none);

    // Write to the file should fail.
    std::error_code ec;
    esl::write_to_file(file, "abcdefg", ec);
    INFO("ec.value=", ec.value(), " ec.message=", ec.message());
    CHECK(static_cast<bool>(ec));
    CHECK_EQ(ec, std::errc::permission_denied);
}

TEST_CASE("read file that does not exist") {
    // Generate the path but not to create the file.
    auto file = tests::new_test_filepath();
    std::error_code ec;
    std::string content;
    esl::read_file_to_string(file, content, ec);
    CHECK(static_cast<bool>(ec));
    CHECK_EQ(ec, std::errc::no_such_file_or_directory);
    CHECK(content.empty());
}

#if !defined(_WIN32)

TEST_CASE("read file that don't have read permission") {
    auto file = tests::new_test_filepath();
    CAPTURE(file);

    // Create the file first.
    std::ofstream out(file);
    out.close();

    // Remove read permission.
    REQUIRE((fs::status(file).permissions() & fs::perms::owner_read) != fs::perms::none);
    fs::permissions(file, fs::perms::owner_read, fs::perm_options::remove);
    REQUIRE((fs::status(file).permissions() & fs::perms::owner_read) == fs::perms::none);

    // Read file should fail.
    std::error_code ec;
    std::string content;
    esl::read_file_to_string(file, content, ec);
    INFO("ec.value=", ec.value(), " ec.message=", ec.message());
    CHECK(static_cast<bool>(ec));
    CHECK_EQ(ec, std::errc::permission_denied);
}

TEST_CASE("read a special file") {
    const std::filesystem::path file{"/proc/cgroups"};
    REQUIRE(std::filesystem::exists(file));
    REQUIRE_EQ(std::filesystem::file_size(file), 0U);
    std::string content;
    esl::read_file_to_string(file.native(), content);
    CHECK_FALSE(content.empty());
    CAPTURE(content);
    CHECK(esl::strings::starts_with(content, "#subsys_name"));
}

#endif

TEST_SUITE_END();

} // namespace
