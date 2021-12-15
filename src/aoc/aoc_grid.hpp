//
// Copyright (c) 2021 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef AOC_GRID_HPP
#define AOC_GRID_HPP

#include <aoc_range.hpp>
#include <aoc_vec.hpp>

#include <compare>

namespace aoc {

template <typename Range>
auto grid_rows(Range& r, int width) noexcept
{
    return r | rv::chunk(width);
}

template <typename Range>
auto grid_row(Range& r, int width, int row) noexcept
{
    return grid_rows(r, width)[row];
}

template <typename Range>
auto grid_col(Range& r, int width, int col) noexcept
{
    return r | rv::drop(col) | rv::stride(width);
}

template <typename Range>
auto grid_cols(Range& r, int width) noexcept
{
    return rv::ints(0, width) |
           rv::transform([&r, width](int i) { return grid_col(r, width, i); });
}

template <typename Base>
class subgrid_view {
   public:
    subgrid_view(Base& base, vec2<int> top_left, vec2<int> dimensions) noexcept
        : base_{base}, r_{top_left, dimensions}
    {
    }

    auto row(int y) const noexcept
    {
        return base_.row(r_.top_left.y + y) | rv::drop(r_.top_left.x) |
               rv::take(r_.dimensions.x);
    }

    auto rows() const noexcept
    {
        return rv::ints(0, r_.dimensions.y) |
               rv::transform([&](int y) { return row(y); });
    }

    auto col(int x) const noexcept
    {
        return base_.col(r_.top_left.x + x) | rv::drop(r_.top_left.y) |
               rv::take(r_.dimensions.y);
    }

    auto cols() const noexcept
    {
        return rv::ints(0, r_.dimensions.x) |
               rv::transform([&](int x) { return col(x); });
    }

    auto data() const noexcept
    {
        auto at_point{[sub = *this](vec2<int> p) -> decltype(base_[p])& {
            return sub.base_[p];
        }};
        return r_.all_points() | rv::transform(at_point);
    }  // namespace aoc

    auto area() const noexcept { return r_; }
    auto width() const noexcept { return r_.dimensions.x; }
    auto height() const noexcept { return r_.dimensions.y; }

    auto& operator[](vec2<int> index) const noexcept
    {
        return base_[index + r_.top_left];
    }

   private:
    Base& base_;
    rect<int> r_;
};

template <typename Range, int width>
class grid_adapter {
   public:
    grid_adapter(Range& range) noexcept : range_(&range) {}

    auto row(int y) const noexcept { return grid_row(*range_, width, y); }
    auto rows() const noexcept { return grid_rows(*range_, width); }
    auto col(int x) const noexcept { return grid_col(*range_, width, x); }
    auto cols() const noexcept { return grid_cols(*range_, width); }

    auto data() noexcept { return rv::all(*range_); }

    auto& operator[](vec2<int> index) const noexcept
    {
        return rows()[index.y][index.x];
    }

    subgrid_view<grid_adapter> subgrid(rect<int> r) noexcept
    {
        return {*this, r.top_left, r.dimensions};
    }

   private:
    Range* range_;
};

template <typename Range>
class dynamic_grid_adapter {
   public:
    dynamic_grid_adapter(Range& range, int width) noexcept
        : range_(&range),
          width_(width),
          height_(static_cast<int>(r::distance(range)) / width)
    {
    }

    auto row(int y) const noexcept { return grid_row(*range_, width_, y); }
    auto rows() const noexcept { return grid_rows(*range_, width_); }
    auto col(int x) const noexcept { return grid_col(*range_, width_, x); }
    auto cols() const noexcept { return grid_cols(*range_, width_); }

    int width() const noexcept { return width_; }
    int height() const noexcept { return height_; }
    rect<int> area() const noexcept { return {{0, 0}, {width_, height_}}; }

    auto data() noexcept { return rv::all(*range_); }

    auto& operator[](vec2<int> index) const noexcept
    {
        return rows()[index.y][index.x];
    }

    // subgrid_view<grid_adapter> subgrid(rect<int> r) noexcept
    // {
    //     return {*this, r.top_left, r.dimensions};
    // }

   private:
    Range* range_;
    int width_;
    int height_;
};

template <typename Value, int width, int height>
class static_grid
    : public grid_adapter<
          std::array<Value, static_cast<std::size_t>(width* height)>,
          width> {
    static constexpr auto size{width * height};
    static_assert(size > 0, "size must be greater than 0");
    static constexpr auto allowed_size{static_cast<std::size_t>(size)};

   public:
    static constexpr rect<int> area{{0, 0}, {width, height}};
    static_grid() noexcept
        : grid_adapter<std::array<Value, allowed_size>, width>{data_}, data_{0}
    {
    }
    std::array<Value, allowed_size> data_;
};

template <typename Value, int size>
class heap_data {
    static_assert(size > 0, "size must be greater than 0");
    static constexpr auto allowed_size{static_cast<std::size_t>(size)};

   public:
    heap_data() noexcept
        : data_{std::make_unique<std::array<Value, allowed_size>>()}
    {
        r::fill(*data_, Value{});
    }
    auto begin() noexcept { return data_->begin(); }
    auto end() noexcept { return data_->end(); }

   private:
    std::unique_ptr<std::array<Value, allowed_size>> data_;
};

template <typename Value, int width, int height>
class heap_grid : public grid_adapter<heap_data<Value, width * height>, width> {
   public:
    static constexpr rect<int> area{{0, 0}, {width, height}};
    heap_grid() noexcept
        : grid_adapter<heap_data<Value, width * height>, width>{data_}, data_{}
    {
    }

    heap_data<Value, width * height> data_;
};

}  // namespace aoc

#endif  // AOC_GRID_HPP
