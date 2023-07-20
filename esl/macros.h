// Copyright (c) 2023 Kingsley Chen <kingsamchen at gmail dot com>
// This file is subject to the terms of license that can be found
// in the LICENSE file.

#pragma once

#if !defined(__COUNTER__)
#error "Requires __COUNTER__"
#endif

#define ESL_CONCAT_IMPL(a, b)  a##b
#define ESL_CONCAT(a, b)       ESL_CONCAT_IMPL(a, b)
#define ESL_ANONYMOUS_VAR(tag) ESL_CONCAT(ESL_CONCAT(ESL_CONCAT(tag, __LINE__), _), __COUNTER__)
