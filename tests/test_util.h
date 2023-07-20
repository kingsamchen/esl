// Copyright (c) 2023 Kingsley Chen <kingsamchen at gmail dot com>
// This file is subject to the terms of license that can be found
// in the LICENSE file.

#pragma once

#include <chrono>
#include <filesystem>
#include <random>
#include <string>

namespace tests {

namespace fs = std::filesystem;

inline std::string new_test_filepath() {
    auto tmp_dir = fs::temp_directory_path();
    auto ts = std::chrono::system_clock::now().time_since_epoch().count();
    auto rnd = std::random_device{}();
    std::string filename{"test_"};
    filename.append(std::to_string(ts)).append("_").append(std::to_string(rnd)).append(".txt");
    return (tmp_dir / filename).string();
}

} // namespace tests
