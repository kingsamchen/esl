// Copyright (c) 2021 Kingsley Chen <kingsamchen at gmail dot com>
// This file is subject to the terms of license that can be found
// in the LICENSE file.

#pragma once

#ifndef ESL_SCOPE_GUARD
#define ESL_SCOPE_GUARD

#include <cstddef>
#include <exception>
#include <functional>
#include <type_traits>
#include <utility>

#include "esl/macros.h"

namespace esl {
namespace detail {

// A scope_guard instance is neither copyable nor movable, but guaranteed copy elision makes
// it able to be returned from a function.

class scope_guard_base {
public:
    ~scope_guard_base() = default;

    scope_guard_base(const scope_guard_base&) = delete;

    scope_guard_base(scope_guard_base&&) = delete;

    scope_guard_base& operator=(const scope_guard_base&) = delete;

    scope_guard_base& operator=(scope_guard_base&&) = delete;

    void dismiss() noexcept {
        dismissed_ = true;
    }

protected:
    scope_guard_base() noexcept = default;

    bool dismissed() const noexcept {
        return dismissed_;
    }

    static scope_guard_base make_empty_scope_guard() noexcept {
        return scope_guard_base{};
    }

private:
    bool dismissed_{false};
};

// Invoking the guard function must not throw.
template<typename Fn>
class scope_guard : public scope_guard_base {
public:
    explicit scope_guard(const Fn& fn) noexcept(std::is_nothrow_copy_constructible_v<Fn>)
        : scope_guard(fn,
                      make_failsafe(std::is_nothrow_copy_constructible<Fn>{}, &fn)) {}

    explicit scope_guard(Fn& fn) noexcept(std::is_nothrow_copy_constructible_v<Fn>)
        : scope_guard(std::as_const(fn),
                      make_failsafe(std::is_nothrow_copy_constructible<Fn>{}, &fn)) {}

    explicit scope_guard(Fn&& fn) noexcept(std::is_nothrow_move_constructible_v<Fn>)
        : scope_guard(std::move_if_noexcept(fn),
                      make_failsafe(std::is_nothrow_move_constructible<Fn>{}, &fn)) {}

    ~scope_guard() {
        if (!dismissed()) {
            guard_fn_();
        }
    }

    scope_guard(const scope_guard&) = delete;

    scope_guard(scope_guard&&) = delete;

    scope_guard& operator=(const scope_guard&) = delete;

    scope_guard& operator=(scope_guard&&) = delete;

    void* operator new(std::size_t) = delete;

    void operator delete(void*) = delete;

private:
    template<typename F>
    scope_guard(F&& fn, scope_guard_base&& failsafe)
        : scope_guard_base(),
          guard_fn_(std::forward<F>(fn)) {
        failsafe.dismiss();
    }

    static scope_guard_base make_failsafe(std::true_type /* nothrow */, const void*) noexcept {
        return make_empty_scope_guard();
    }

    template<typename F>
    static auto make_failsafe(std::false_type /* nothrow */, F* pfn) noexcept {
        return scope_guard<decltype(std::ref(*pfn))>(std::ref(*pfn));
    }

private:
    Fn guard_fn_;
};

template<typename F>
using scope_guard_decay = scope_guard<std::decay_t<F>>;

struct scope_guard_on_exit {};

template<typename F>
scope_guard_decay<F> operator+(scope_guard_on_exit, F&& fn) noexcept(
        noexcept(scope_guard_decay<F>(std::forward<F>(fn)))) {
    return scope_guard_decay<F>(std::forward<F>(fn));
}

template<typename Fn, bool ExecuteOnException>
class scope_guard_for_new_exception {
public:
    explicit scope_guard_for_new_exception(const Fn& fn) noexcept(
            std::is_nothrow_copy_constructible_v<Fn>)
        : guard_(fn) {}

    explicit scope_guard_for_new_exception(Fn& fn) noexcept(
            std::is_nothrow_copy_constructible_v<Fn>)
        : guard_(fn) {}

    explicit scope_guard_for_new_exception(Fn&& fn) noexcept(
            std::is_nothrow_move_constructible_v<Fn>)
        : guard_(std::move(fn)) {}

    ~scope_guard_for_new_exception() {
        if ((exception_cnt_ < std::uncaught_exceptions()) != ExecuteOnException) {
            guard_.dismiss();
        }
    }

    scope_guard_for_new_exception(const scope_guard_for_new_exception&) = delete;

    scope_guard_for_new_exception(scope_guard_for_new_exception&&) = delete;

    scope_guard_for_new_exception& operator=(const scope_guard_for_new_exception&) = delete;

    scope_guard_for_new_exception& operator=(scope_guard_for_new_exception&&) = delete;

    void* operator new(std::size_t) = delete;

    void operator delete(void*) = delete;

private:
    scope_guard<Fn> guard_;
    int exception_cnt_{std::uncaught_exceptions()};
};

template<typename Fn, bool ExecuteOnException>
using scope_guard_for_new_exception_decay =
        scope_guard_for_new_exception<std::decay_t<Fn>, ExecuteOnException>;

struct scope_guard_on_fail {};

template<typename F>
scope_guard_for_new_exception_decay<F, true> operator+(scope_guard_on_fail, F&& fn) noexcept(
        noexcept(scope_guard_for_new_exception_decay<F, true>(std::forward<F>(fn)))) {
    return scope_guard_for_new_exception_decay<F, true>(std::forward<F>(fn));
}

struct scope_guard_on_success {};

template<typename F>
scope_guard_for_new_exception_decay<F, false> operator+(scope_guard_on_success, F&& fn) noexcept(
        noexcept(scope_guard_for_new_exception_decay<F, false>(std::forward<F>(fn)))) {
    return scope_guard_for_new_exception_decay<F, false>(std::forward<F>(fn));
}

} // namespace detail

template<typename F>
[[nodiscard]] detail::scope_guard_decay<F> make_scope_guard(F&& fn) noexcept(
        noexcept(detail::scope_guard_decay<F>(std::forward<F>(fn)))) {
    return detail::scope_guard_decay<F>(std::forward<F>(fn));
}

} // namespace esl

// TODO: Use block NOLINT only once upgraded to clang-tidy 15.
// NOLINTBEGIN(bugprone-macro-parentheses)

#define ESL_MAKE_EXIT_GUARD \
    ::esl::detail::scope_guard_on_exit{} + [&]() noexcept // NOLINT

#define ESL_ON_SCOPE_EXIT                                 \
    [[maybe_unused]] auto ESL_ANONYMOUS_VAR(scope_exit) = \
            ::esl::detail::scope_guard_on_exit{} + [&]() noexcept // NOLINT

#define ESL_ON_SCOPE_FAIL                                 \
    [[maybe_unused]] auto ESL_ANONYMOUS_VAR(scope_fail) = \
            ::esl::detail::scope_guard_on_fail{} + [&]() noexcept // NOLINT

#define ESL_ON_SCOPE_SUCCESS                                 \
    [[maybe_unused]] auto ESL_ANONYMOUS_VAR(scope_success) = \
            ::esl::detail::scope_guard_on_success{} + [&]() noexcept // NOLINT

// NOLINTEND(bugprone-macro-parentheses)

#endif // ESL_SCOPE_GUARD
