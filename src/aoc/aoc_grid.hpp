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

#include <algorithm>
#include <array>
#include <compare>
#include <memory>
#include <span>
#include <tuple>

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
    subgrid_view(Base& base, vec2<int> rect_base, vec2<int> dimensions) noexcept
        : base_{base}, r_{rect_base, dimensions}
    {
    }

    auto row(int y) const noexcept
    {
        return base_.row(r_.base.y + y) | rv::drop(r_.base.x) |
               rv::take(r_.dimensions.x);
    }

    auto rows() const noexcept
    {
        return rv::ints(0, r_.dimensions.y) |
               rv::transform([&](int y) { return row(y); });
    }

    auto col(int x) const noexcept
    {
        return base_.col(r_.base.x + x) | rv::drop(r_.base.y) |
               rv::take(r_.dimensions.y);
    }

    auto cols() const noexcept
    {
        return rv::ints(0, r_.dimensions.x) |
               rv::transform([&](int x) { return col(x); });
    }

    auto data() noexcept
    {
        auto at_point{[sub = *this](vec2<int> p) -> decltype(base_[p])& {
            return sub.base_[p];
        }};
        return r_.all_points() | rv::transform(at_point);
    }  // namespace aoc

    // TODO const-correctness is all wrong
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
        return base_[index + r_.base];
    }

   private:
    Base& base_;
    rect<int> r_;
};

template <typename Range, int Width>
class grid_adapter {
   public:
    grid_adapter(Range& range) noexcept : range_(&range) {}

    auto row(int y) const noexcept { return grid_row(*range_, Width, y); }
    auto rows() const noexcept { return grid_rows(*range_, Width); }
    auto col(int x) const noexcept { return grid_col(*range_, Width, x); }
    auto cols() const noexcept { return grid_cols(*range_, Width); }

    auto data() noexcept { return rv::all(*range_); }
    auto data() const noexcept { return rv::all(*range_); }

    auto& operator[](vec2<int> index) const noexcept
    {
        return rows()[index.y][index.x];
    }

    subgrid_view<grid_adapter> subgrid(rect<int> r) noexcept
    {
        return {*this, r.base, r.dimensions};
    }

    subgrid_view<const grid_adapter> subgrid(rect<int> r) const noexcept
    {
        return {*this, r.base, r.dimensions};
    }

   protected:
    // Protected is a hack to make ill-concieved copy assignment work
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

    dynamic_grid_adapter(Range& range, int width, int height) noexcept
        : range_(&range), width_(width), height_(height)
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

    subgrid_view<dynamic_grid_adapter> subgrid(rect<int> r) noexcept
    {
        return {*this, r.base, r.dimensions};
    }

   protected:
    // Protected is a hack to make ill-concieved copy assignment work
    Range* range_;
    int width_;
    int height_;
};

template <typename Value, int width_, int height_>
class static_grid
    : public grid_adapter<
          std::array<Value, static_cast<std::size_t>(width_* height_)>,
          width_> {
    static constexpr auto size{width_ * height_};
    static_assert(size > 0, "size must be greater than 0");
    static constexpr auto allowed_size{static_cast<std::size_t>(size)};

   public:
    static constexpr rect<int> area{{0, 0}, {width_, height_}};
    static_grid() noexcept
        : grid_adapter<std::array<Value, allowed_size>, width_>{data_}, data_{0}
    {
    }

    static_grid(const static_grid& other) noexcept
        : grid_adapter<std::array<Value, allowed_size>, width_>{data_},
          data_{other.data_}
    {
        this->range_ = &data_;
    }

    int width() const noexcept { return width_; }
    int height() const noexcept { return height_; }

    static_grid& operator=(const static_grid& other) noexcept
    {
        data_ = other.data_;
        this->range_ = &data_;
        return *this;
    }

    friend auto operator<=>(const static_grid& lhs,
                            const static_grid& rhs) noexcept
    {
        return lhs.data_ <=> rhs.data_;
    }

    friend bool operator==(const static_grid& lhs,
                           const static_grid& rhs) noexcept
    {
        return lhs.data_ == rhs.data_;
    }

   private:
    std::array<Value, allowed_size> data_;
};

template <typename Value, int size>
class heap_data {
    static_assert(size > 0, "size must be greater than 0");
    static constexpr auto allowed_size{static_cast<std::size_t>(size)};

   public:
    heap_data() : data_{std::make_unique<std::array<Value, allowed_size>>()}
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
        // FIXME: passign `data_` by reference before it's initialized is
        // undefined behavior
        : grid_adapter<heap_data<Value, width * height>, width>{data_}, data_{}
    {
    }

    heap_data<Value, width * height> data_;
};

template <typename Value>
class dynamic_heap_data {
   public:
    dynamic_heap_data(std::size_t size)
        : data_{std::make_unique<Value[]>(size)}, size_{size}
    {
        r::fill(std::span{data_.get(), size_}, Value{0});
    }

    // dynamic_heap_data(dynamic_heap_data&& other) noexcept
    //     : data_{std::move(other.data_)}, size_{other.size_}
    // {
    // }

    dynamic_heap_data(const dynamic_heap_data& other) noexcept
        : data_{std::make_unique<Value[]>(other.size_)}, size_{other.size_}
    {
        std::copy(other.data_.get(), other.data_.get() + size_, data_.get());
    }

    // dynamic_heap_data& operator=(dynamic_heap_data&& other) noexcept
    // {
    //     data_ = std::move(other.data_);
    //     size_ = other.size_;
    //     return *this;
    // }

    dynamic_heap_data& operator=(const dynamic_heap_data& other) noexcept
    {
        data_ = std::make_unique<Value[]>(other.size_);
        size_ = other.size_;
        std::copy(other.data_.get(), other.data_.get() + size_, data_.get());
        return *this;
    }

    auto begin() const noexcept { return data_.get(); }
    auto end() const noexcept { return begin() + size_; }

    friend auto operator<=>(const dynamic_heap_data& lhs,
                            const dynamic_heap_data& rhs) noexcept
    {
        return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(),
                                            rhs.end());
    }

    friend auto operator==(const dynamic_heap_data& lhs,
                           const dynamic_heap_data& rhs) noexcept
    {
        return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
    }

   private:
    std::unique_ptr<Value[]> data_;
    std::size_t size_;
};

template <typename Value>
class dynamic_grid : public dynamic_grid_adapter<dynamic_heap_data<Value>> {
   public:
    dynamic_grid(int width, int height) noexcept
        // FIXME: passign `data_` by reference before it's initialized is
        // undefined behavior
        : dynamic_grid_adapter<dynamic_heap_data<Value>>{data_, width, height},
          data_{static_cast<std::size_t>(width * height)}
    {
    }

    dynamic_grid(const dynamic_grid& other) noexcept
        : dynamic_grid_adapter<dynamic_heap_data<Value>>{data_, other.width(),
                                                         other.height()},
          data_{other.data_}
    {
    }

    dynamic_grid& operator=(const dynamic_grid& other) noexcept
    {
        data_ = other.data_;
        this->range_ = &data_;
        this->width_ = other.width_;
        this->height_ = other.height_;
        return *this;
    }

    friend auto operator<=>(const dynamic_grid& lhs,
                            const dynamic_grid& rhs) noexcept
    {
        return std::tie(lhs.width_, lhs.height_, lhs.data_) <=>
               std::tie(rhs.width_, rhs.height_, rhs.data_);
    }

    friend bool operator==(const dynamic_grid& lhs,
                           const dynamic_grid& rhs) noexcept
    {
        return std::tie(lhs.width_, lhs.height_, lhs.data_) ==
               std::tie(rhs.width_, rhs.height_, rhs.data_);
    }

    // TODO: make movable, untangle adapter mess
    // TODO: make assignment operators work
    // adapter should be a member variable and assignment should swap it for a
    // new one

    dynamic_heap_data<Value> data_;
};

}  // namespace aoc

#endif  // AOC_GRID_HPP
