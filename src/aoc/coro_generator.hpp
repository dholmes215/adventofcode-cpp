//
// Copyright (C) 2020-2022 various cppreference.com editors:
// https://en.cppreference.com/mwiki/index.php?title=cpp/coroutine/coroutine_handle&action=history
//
// Distributed under the CC BY-SA 3.0 license:
// https://creativecommons.org/licenses/by-sa/3.0/
//

// This class template was adapted from the example code at
// https://en.cppreference.com/w/cpp/coroutine/coroutine_handle

#ifndef CORO_GENERATOR_HPP
#define CORO_GENERATOR_HPP

#include <coroutine>
#include <optional>

template <std::movable T>
class Generator {
   public:
    struct promise_type {
        Generator<T> get_return_object()
        {
            return Generator{Handle::from_promise(*this)};
        }
        static std::suspend_always initial_suspend() noexcept { return {}; }
        static std::suspend_always final_suspend() noexcept { return {}; }
        std::suspend_always yield_value(T value) noexcept
        {
            current_value = std::move(value);
            return {};
        }
        // Disallow co_await in generator coroutines.
        void await_transform() = delete;
        void return_void() {}
        [[noreturn]] static void unhandled_exception() { throw; }

        std::optional<T> current_value;
    };

    using Handle = std::coroutine_handle<promise_type>;

    explicit Generator(const Handle coroutine) : m_coroutine{coroutine} {}

    Generator() = default;
    ~Generator()
    {
        if (m_coroutine) {
            m_coroutine.destroy();
        }
    }

    Generator(const Generator&) = delete;
    Generator& operator=(const Generator&) = delete;

    Generator(Generator&& other) noexcept : m_coroutine{other.m_coroutine}
    {
        other.m_coroutine = {};
    }
    Generator& operator=(Generator&& other) noexcept
    {
        if (this != &other) {
            if (m_coroutine) {
                m_coroutine.destroy();
            }
            m_coroutine = other.m_coroutine;
            other.m_coroutine = {};
        }
        return *this;
    }

    // Range-based for loop support.
    class Iter {
       public:
        // Thanks to wreien from #include<C++> Discord for fixing this class
        using value_type = T;
        using difference_type = std::ptrdiff_t;

        Iter& operator++()
        {
            m_coroutine.resume();
            return *this;
        }
        void operator++(int) { operator++(); }
        const T& operator*() const
        {
            return *m_coroutine.promise().current_value;
        }
        bool operator==(std::default_sentinel_t) const
        {
            return !m_coroutine || m_coroutine.done();
        }

        explicit Iter(const Handle coroutine) : m_coroutine{coroutine} {}

       private:
        Handle m_coroutine;
    };

    Iter begin()
    {
        if (m_coroutine) {
            m_coroutine.resume();
        }
        return Iter{m_coroutine};
    }
    std::default_sentinel_t end() { return {}; }

   private:
    Handle m_coroutine;
};

#endif  // CORO_GENERATOR_HPP
