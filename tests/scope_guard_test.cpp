// Copyright (c) 2021 Kingsley Chen <kingsamchen at gmail dot com>
// This file is subject to the terms of license that can be found
// in the LICENSE file.

#include <functional>
#include <stdexcept>
#include <vector>

#include "doctest/doctest.h"

#include "esl/scope_guard.h"

using esl::make_scope_guard;

namespace {

int trivial_fn() {
    return 0;
}

struct incrementer {
    int& value;

    explicit incrementer(int& initial)
        : value(initial) {}

    void operator()() {
        ++value;
    }
};

TEST_SUITE_BEGIN("scope_guard");

TEST_CASE("non-copyable/non-movable") {
    auto fn = [] {
        (void)0;
    };
    using fn_t = decltype(fn);
    REQUIRE(std::is_copy_constructible_v<fn_t>);
    REQUIRE(std::is_move_constructible_v<fn_t>);

    [[maybe_unused]] auto guard = make_scope_guard(fn);
    using guard_t = decltype(guard);
    CHECK_FALSE(std::is_copy_constructible_v<guard_t>);
    CHECK_FALSE(std::is_move_constructible_v<guard_t>);
}

TEST_CASE("support different types of guard function") {
    SUBCASE("normal function") {
        [[maybe_unused]] auto guard = make_scope_guard(trivial_fn);
    }

    SUBCASE("function pointer") {
        [[maybe_unused]] auto guard = make_scope_guard(&trivial_fn);
    }

    SUBCASE("bind with member function") {
        std::vector<int> v;
        auto pmfn = static_cast<void (std::vector<int>::*)(const int&)>(&decltype(v)::push_back);

        SUBCASE("pass instance by reference") {
            REQUIRE(v.empty());
            {
                // NOLINTNEXTLINE(modernize-avoid-bind)
                [[maybe_unused]] auto guard = make_scope_guard(std::bind(pmfn, std::ref(v), 42));
            }
            CHECK_EQ(v.size(), 1);
        }

        SUBCASE("pass instance by value") {
            REQUIRE(v.empty());
            {
                // NOLINTNEXTLINE(modernize-avoid-bind)
                [[maybe_unused]] auto guard = make_scope_guard(std::bind(pmfn, v, 42));
            }
            CHECK_EQ(v.size(), 0);
        }
    }

    SUBCASE("lambda") {
        std::vector<int> v;

        SUBCASE("capture by reference") {
            REQUIRE(v.empty());
            {
                [[maybe_unused]] auto guard = make_scope_guard([&] {
                    v.push_back(42);
                });
            }
            CHECK_EQ(v.size(), 1);
        }

        SUBCASE("capture by value") {
            REQUIRE(v.empty());
            {
                [[maybe_unused]] auto guard = make_scope_guard([v]() mutable {
                    v.push_back(42);
                });
            }
            CHECK_EQ(v.size(), 0);
        }

        SUBCASE("reuse a mutable lambda") {
            REQUIRE(v.empty());
            auto op = [&v]() mutable {
                v.push_back(42);
            };

            {
                [[maybe_unused]] auto guard = make_scope_guard(op);
            }
            CHECK_EQ(v.size(), 1);

            {
                [[maybe_unused]] auto guard = make_scope_guard(op);
            }
            CHECK_EQ(v.size(), 2);
        }
    }

    SUBCASE("function object") {
        int n = 41; // NOLINT(readability-magic-numbers)

        SUBCASE("function object") {
            {
                incrementer incr(n);
                [[maybe_unused]] auto guard = make_scope_guard(incr);
            }
            CHECK_EQ(n, 42);
        }

        SUBCASE("temporary function object") {
            {
                [[maybe_unused]] auto guard = make_scope_guard(incrementer(n));
            }
            CHECK_EQ(n, 42);
        }
    }
}

TEST_CASE("dismiss the guard") {
    std::vector<int> v;
    auto append = [&v](int new_val, bool success) {
        v.push_back(new_val);
        [[maybe_unused]] auto guard = make_scope_guard([&v] {
            if (!v.empty()) {
                v.pop_back();
            }
        });

        if (success) {
            guard.dismiss();
        }
    };

    append(42, true);
    CHECK_EQ(v.size(), 1);
    CHECK_EQ(v.at(0), 42);

    append(1024, false);
    CHECK_EQ(v.size(), 1);
    CHECK_EQ(v.at(0), 42);
}

TEST_CASE("handy macros") {
    SUBCASE("on scope exit") {
        int i = 0;

        SUBCASE("exit normally") {
            REQUIRE_EQ(i, 0);
            {
                ESL_ON_SCOPE_EXIT {
                    ++i;
                };
                CHECK_EQ(i, 0);
            }
            CHECK_EQ(i, 1);
        }

        SUBCASE("exit by exception") {
            REQUIRE_EQ(i, 0);
            try {
                ESL_ON_SCOPE_EXIT {
                    ++i;
                };
                CHECK_EQ(i, 0);
                throw std::runtime_error("vala");
            } catch (...) {
            }
            CHECK_EQ(i, 1);
        }
    }

    SUBCASE("on scope fail") {
        bool guard_executed = false;

        SUBCASE("exit normally") {
            {
                ESL_ON_SCOPE_FAIL {
                    guard_executed = true;
                };
            }
            CHECK_FALSE(guard_executed);
        }

        SUBCASE("exception occurs") {
            try {
                ESL_ON_SCOPE_FAIL {
                    guard_executed = true;
                };
                throw std::runtime_error("vala");
            } catch (...) {
            }
            CHECK(guard_executed);
        }
    }

    SUBCASE("on scope success") {
        bool guard_executed = false;

        SUBCASE("exit normally") {
            {
                ESL_ON_SCOPE_SUCCESS {
                    guard_executed = true;
                };
            }
            CHECK(guard_executed);
        }

        SUBCASE("exception occurs") {
            try {
                ESL_ON_SCOPE_SUCCESS {
                    guard_executed = true;
                };
                throw std::runtime_error("omg");
            } catch (...) {
            }
            CHECK_FALSE(guard_executed);
        }
    }
}

TEST_SUITE_END();

} // namespace
