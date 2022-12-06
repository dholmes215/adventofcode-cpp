//
// Copyright (c) 2022 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef TINY_VECTOR_HPP
#define TINY_VECTOR_HPP

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <memory>
#include <stdexcept>
#include <type_traits>

namespace aoc {

// TODO: constexpr?

template <typename T>
class alignas(8) tiny_vector {
    static_assert(sizeof(T) == 1, "tiny_vector can only store 1-byte objects");

   public:
    using value_type = T;
    using size_type = std::size_t;
    using reference = T&;
    using const_reference = const T&;
    using pointer = T*;
    using const_pointer = const T*;
    using iterator = T*;
    using const_iterator = const T*;

    tiny_vector() noexcept {}

    tiny_vector(size_type count, const T& value)
        : size_(static_cast<std::uint8_t>(count))
    {
        check_capacity_(
            count,
            "Cannot construct tiny_vector with size greater than capacity");
        for (size_type i{0}; i < count; i++) {
            std::construct_at(data() + i, value);
        }
    }

    explicit tiny_vector(size_type count)
        : size_(static_cast<std::uint8_t>(count))
    {
        check_capacity_(
            count,
            "Cannot construct tiny_vector with size greater than capacity");
        for (size_type i{0}; i < count; i++) {
            std::construct_at(data() + i);
        }
    }

    template <typename InputIt>
    tiny_vector(InputIt first, InputIt last)
    {
#if defined(__GNUC__) && !defined(__llvm__) && !defined(__INTEL_COMPILER)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
        auto iter{first};
#pragma GCC diagnostic pop
#else
        auto iter{first};
#endif
        while (iter != last) {
            push_back(*iter++);
        }
    }

    tiny_vector(const tiny_vector& other)
    {
        for (const auto& o : other) {
            push_back(o);
        }
    }

    tiny_vector(tiny_vector&& other)
    {
        for (auto& o : other) {
            push_back(std::move(o));
        }
        other.clear();
    }

    explicit tiny_vector(std::initializer_list<T> init)
        : tiny_vector{init.begin(), init.end()}
    {
    }

    ~tiny_vector() noexcept { clear(); }

    tiny_vector& operator=(const tiny_vector& other)
    {
        clear();
        for (const auto& o : other) {
            push_back(o);
        }
        return *this;
    }

    tiny_vector& operator=(tiny_vector&& other)
    {
        clear();
        for (auto& o : other) {
            push_back(std::move(o));
        }
        other.clear();
        return *this;
    }

    // TODO: assign()? required for SequenceContainer

    // Element Access

    reference at(size_type index) noexcept
    {
        if (index < 0 || index >= size()) {
            throw std::out_of_range{"index out of bounds"};
        }
        return (*this)[index];
    }

    const_reference at(size_type index) const noexcept
    {
        if (index < 0 || index >= size()) {
            throw std::out_of_range{"index out of bounds"};
        }
        return (*this)[index];
    }

    reference operator[](size_type index) noexcept
    {
        return *(begin() + index);
    }

    const_reference operator[](size_type index) const noexcept
    {
        return *(begin() + index);
    }

    reference front() noexcept { return (*this)[0]; }

    const_reference front() const noexcept { return (*this)[0]; }

    reference back() noexcept { return (*this)[size() - 1]; }

    const_reference back() const noexcept { return (*this)[size() - 1]; }

    pointer data() noexcept
    {
        return std::launder(reinterpret_cast<pointer>(storage_.data()));
    }

    const_pointer data() const noexcept
    {
        return std::launder(reinterpret_cast<const_pointer>(storage_.data()));
    }

    // Iterators

    iterator begin() noexcept
    {
        static_assert(std::is_same_v<pointer, iterator>);
        return data();
    }

    const_iterator begin() const noexcept
    {
        static_assert(std::is_same_v<const_pointer, const_iterator>);
        return data();
    }

    iterator end() noexcept
    {
        static_assert(std::is_same_v<pointer, iterator>);
        return data() + size();
    }

    const_iterator end() const noexcept
    {
        static_assert(std::is_same_v<const_pointer, const_iterator>);
        return data() + size();
    }

    // Capacity

    bool empty() const noexcept { return size() == 0; }
    size_type size() const noexcept { return size_; }
    size_type max_size() const noexcept { return capacity_; }
    size_type capacity() const noexcept { return capacity_; }

    // Modifiers

    void clear() noexcept
    {
        std::destroy_n(data(), size());
        size_ = 0;
    }

    // TODO: insert
    // TODO: emplace
    // TODO: erase

    // copy push_back
    void push_back(const T& t)
    {
        check_capacity_(size() + 1,
                        "Cannot push_back because tiny_vector is full");
        std::construct_at(end_pointer_(), t);
        size_++;
    }

    // move push_back
    void push_back(T&& t)
    {
        check_capacity_(size() + 1,
                        "Cannot push_back because tiny_vector is full");
        std::construct_at(end_pointer_(), std::move(t));
        size_++;
    }

    void pop_back() noexcept
    {
        if (empty()) {
            // For STL containers, calling pop_back() when empty would be UB
            return;
        }
        size_--;
        std::destroy_at(end_pointer_());
    }

    // TODO: resize

    void resize(size_type count)
    {
        check_capacity_(
            count, "Cannot resize tiny_vector to size greater than capacity");
        if (count > size()) {
            for (size_type i{size()}; i < count; i++) {
                std::construct_at(data() + i);
            }
        }
        else {
            std::destroy_n(data() + count, size() - count);
        }
        size_ = static_cast<std::uint8_t>(count);
    }

    void resize(size_type count, const value_type& value)
    {
        check_capacity_(
            count, "Cannot resize tiny_vector to size greater than capacity");
        if (count > size()) {
            for (size_type i{size()}; i < count; i++) {
                std::construct_at(data() + i, value);
            }
        }
        else {
            std::destroy_n(data() + count, size() - count);
        }
        size_ = count;
    }

    // TODO: swap

    // Non-member functions

    friend bool operator==(const tiny_vector& lhs,
                           const tiny_vector& rhs) noexcept
    {
        return std::ranges::equal(lhs, rhs);
    }

    // TODO: operator<=>
    // TODO: std::swap
    // TODO: erase and erase_if

    class capacity_error : public std::runtime_error {
       public:
        capacity_error(const char* msg) : runtime_error{msg} {}
    };

   private:
    static constexpr size_type capacity_{7};
    std::array<std::byte, capacity_> storage_;
    std::uint8_t size_{0};

    pointer end_pointer_() noexcept { return data() + size(); }

    void check_capacity_(size_type requested_size,
                         const char* failure_msg) const
    {
        if (requested_size > capacity()) {
            throw capacity_error(failure_msg);
        }
    }
};

static_assert(sizeof(tiny_vector<char>) == 8,
              "sizeof(tiny_vector) is unexpectedly not 8");
static_assert(alignof(tiny_vector<char>) == 8,
              "alignof(tiny_vector) is unexpectedly not 8");
static_assert(std::is_polymorphic_v<tiny_vector<char>> == false,
              "tiny_vector was not intended to be polymorphic");

}  // namespace aoc

#endif  // TINY_VECTOR_HPP
