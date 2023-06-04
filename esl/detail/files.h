// Copyright (c) 2023 Kingsley Chen <kingsamchen at gmail dot com>
// This file is subject to the terms of license that can be found
// in the LICENSE file.

#pragma once

#ifndef ESL_DETAIL_FILES_H_
#define ESL_DETAIL_FILES_H_

#include <cstdint>
#include <cstdio>

#if defined(_WIN32)
#include <Windows.h>
#include <fileapi.h>
#include <io.h>
#else
#include <sys/stat.h>
#endif

namespace esl::detail {

// Returns the file size for an opened file stream.
// Returns 0 if an error occurred.
// The file size of a special file (e.g. files under /proc) may also be 0 even succeeded.
inline std::size_t get_file_size(std::FILE* fp) {
#if defined(_WIN32)
    BY_HANDLE_FILE_INFORMATION file_info{};
    auto fh = reinterpret_cast<HANDLE>( // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
            ::_get_osfhandle(::_fileno(fp)));
    if (!::GetFileInformationByHandle(fh, &file_info)) {
        return 0;
    }
    ULARGE_INTEGER file_size;
    file_size.LowPart = file_info.nFileSizeLow;
    file_size.HighPart = file_info.nFileSizeHigh;
    return file_size.QuadPart > 0 ? file_size.QuadPart : 0;
#else
    struct stat64 file_stat {};
    const int ret = ::fstat64(::fileno(fp), &file_stat);
    return ret == 0 ? static_cast<std::size_t>(file_stat.st_size) : 0;
#endif
}

} // namespace esl::detail

#endif // ESL_DETAIL_FILES_H_
