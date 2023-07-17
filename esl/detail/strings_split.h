// Copyright (c) 2023 Kingsley Chen <kingsamchen at gmail dot com>
// This file is subject to the terms of license that can be found
// in the LICENSE file.

#pragma once

#ifndef ESL_DETAIL_STRINGS_SPLIT_H_
#define ESL_DETAIL_STRINGS_SPLIT_H_

#include <cassert>
#include <cstddef>
#include <iterator>
#include <optional>
#include <stdexcept>
#include <string_view>
#include <type_traits>
#include <vector>

namespace esl::strings::detail {

template<typename Delimiter, typename Predicate>
class split_iterator {
    enum class scan_state {
        end = 0,
        last,
        scanning
    };

public:
    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = std::string_view;
    using reference = const value_type&;
    using pointer = const value_type*;

    // Construct an end iterator, as required by the spec.
    split_iterator() noexcept
        : pos_(std::string_view::npos),
          state_(scan_state::end) {}

    // Construct an explicit end iterator, i.e. bound to a string.
    explicit split_iterator(std::string_view text) noexcept
        : pos_(std::string_view::npos),
          state_(scan_state::end),
          text_(text) {}

    // Construct a begin iterator.
    split_iterator(std::string_view text, std::size_t pos, Delimiter delim, Predicate pred)
        : pos_(pos),
          state_(scan_state::scanning),
          text_(text),
          delimiter_(std::move(delim)),
          predicate_(std::move(pred)) {
        assert(pos_ != std::string_view::npos);
        advance();
    }

    ~split_iterator() = default;

    split_iterator(const split_iterator&) = default;

    split_iterator(split_iterator&&) noexcept = default;

    split_iterator& operator=(const split_iterator&) = default;

    split_iterator& operator=(split_iterator&&) noexcept = default;

    split_iterator& operator++() {
        advance();
        return *this;
    }

    split_iterator operator++(int) {
        split_iterator old(*this);
        ++(*this);
        return old;
    }

    reference operator*() const noexcept {
        return curr_;
    }

    pointer operator->() const noexcept {
        return &curr_;
    }

    friend bool operator==(const split_iterator& lhs, const split_iterator& rhs) noexcept {
        return lhs.pos_ == rhs.pos_ && lhs.state_ == rhs.state_;
    }

    friend bool operator!=(const split_iterator& lhs, const split_iterator& rhs) noexcept {
        return !(lhs == rhs);
    }

private:
    void advance() {
        assert(state_ != scan_state::end);
        assert(delimiter_.has_value());
        assert(predicate_.has_value());

        if (state_ == scan_state::end || !delimiter_.has_value() || !predicate_.has_value()) {
            throw std::logic_error("cannot advance an invalid split_iterator");
        }

        // `delimiter_` and `predicate_` are empty only in end iterator.
        do {
            if (state_ == scan_state::last) {
                pos_ = std::string_view::npos;
                state_ = scan_state::end;
                return;
            }

            // `substr()` call is well behaved even if `delim_start` is `npos` or `pos_`
            // equals to `text_.size()`.
            auto delim_start = delimiter_->find(text_, pos_);
            curr_ = text_.substr(pos_, delim_start - pos_);
            if (delim_start == std::string_view::npos) {
                pos_ = text_.size();
                state_ = scan_state::last;
            } else {
                pos_ = delim_start + delimiter_->size();
            }
        } while (!(*predicate_)(curr_));
    }

private:
    std::size_t pos_;
    scan_state state_;
    std::string_view text_;
    std::string_view curr_;
    std::optional<Delimiter> delimiter_;
    std::optional<Predicate> predicate_;
};

template<typename T, typename = void>
struct has_insert_fn : std::false_type {};

template<typename T>
struct has_insert_fn<
        T,
        std::void_t<
                decltype(std::declval<T>().insert(typename T::iterator{},
                                                  typename T::value_type{}))>>
    : std::true_type {};

template<typename C, typename = void>
struct can_construct_container : std::false_type {};

// Container `C` must:
// 1) have type `value_type` defined, and able to construct a `value_type` from
//    `std::string_view`. This rules out map-like containers.
// 2) have `insert()` function defined for `std::insert_iteartor`. This rules out
//    containers like `std::forward_list`.
// We may make `has_insert_fn<>` an optional requirement in the future provided a custom
// specialized `construct_container` impl is present.
template<typename C>
struct can_construct_container<
        C,
        std::enable_if_t<
                std::conjunction_v<
                        std::is_constructible<typename C::value_type, std::string_view>,
                        has_insert_fn<C>>>>
    : std::true_type {};

template<typename C>
constexpr bool can_construct_container_v = can_construct_container<C>::value;

template<typename Container, typename ValueType>
struct construct_container {
    template<typename SplitView>
    Container operator()(const SplitView& view) const {
        Container container;
        auto it = std::inserter(container, container.end());
        for (const auto& value : view) {
            *it++ = ValueType(value);
        }
        return container;
    }
};

// Optimized for splitting to a `std::vector<std::string_view>`.
// Range insertion with a pair of random access iterators can reduce reallocations
// as possible.
template<typename Allocator>
struct construct_container<std::vector<std::string_view, Allocator>, std::string_view> {
    template<typename SplitView>
    std::vector<std::string_view, Allocator> operator()(const SplitView& view) const {
        std::vector<std::string_view, Allocator> vec;
        constexpr int batch_size = 16;
        std::string_view buf[batch_size];
        for (auto it = view.cbegin(); it != view.cend();) {
            int cnt = 0;
            do {
                buf[cnt++] = *it++;
            } while (cnt < batch_size && it != view.cend());
            vec.insert(vec.end(), std::begin(buf), std::next(std::begin(buf), cnt));
        }
        return vec;
    }
};

// Optimized for splitting to a `std::vector<std::string>`.
// By splitting to a `std::vector<std::string_view>` first, we can reserve enough space in
// the final vector in most cases.
template<typename Allocator>
struct construct_container<std::vector<std::string, Allocator>, std::string> {
    template<typename SplitView>
    std::vector<std::string, Allocator> operator()(const SplitView& view) const {
        auto vec = view.template to<std::vector<std::string_view>>();
        return std::vector<std::string, Allocator>(vec.cbegin(), vec.cend());
    }
};

// `Delimiter` and `Predicate` should be cheap to copy.
template<typename StringType, typename Delimiter, typename Predicate>
class split_view {
public:
    using const_iterator = split_iterator<Delimiter, Predicate>;
    using iterator = const_iterator;

    split_view(StringType text, Delimiter delim, Predicate pred)
        : text_(std::move(text)),
          delimiter_(std::move(delim)),
          predicate_(std::move(pred)) {}

    ~split_view() = default;

    split_view(const split_view&) = default;

    split_view(split_view&&) noexcept = default;

    split_view& operator=(const split_view&) = default;

    split_view& operator=(split_view&&) noexcept = default;

    iterator begin() const {
        return iterator(text_, 0, delimiter_, predicate_);
    }

    const_iterator cbegin() const {
        return begin();
    }

    iterator end() const {
        return iterator(text_);
    }

    const_iterator cend() const {
        return end();
    }

    std::string_view text() const noexcept {
        return text_;
    }

    const Delimiter& delimiter() const noexcept {
        return delimiter_;
    }

    const Predicate& predicate() const noexcept {
        return predicate_;
    }

    template<typename Container,
             std::enable_if_t<can_construct_container_v<Container>, int> = 0>
    Container to() const {
        return construct_container<Container, typename Container::value_type>{}(*this);
    }

private:
    StringType text_;
    Delimiter delimiter_;
    Predicate predicate_;
};

} // namespace esl::strings::detail

#endif // ESL_DETAIL_STRINGS_SPLIT_H_
