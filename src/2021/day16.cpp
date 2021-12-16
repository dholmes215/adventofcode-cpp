//
// Copyright (c) 2020-2021 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <aoc.hpp>
#include <aoc_range.hpp>

#include <fmt/format.h>

#include <cstdint>
#include <memory>
#include <span>
#include <string_view>
#include <vector>

namespace aoc::year2021 {

namespace {

}  // namespace

using bit_t = std::uint8_t;
using bit_range_t = r::any_view<bit_t>;
using bit_iter_t = std::vector<bit_t>::const_iterator;
using bit_span_t = std::span<const bit_t>;
using value_t = std::uint64_t;

auto bit_range(int i, std::size_t count)
{
    auto generator{
        [=]() mutable { return static_cast<bit_t>((i >> --count) & 1); }};
    return rv::generate_n(generator, count);
}

auto hex_to_bit_range(char hex)
{
    const int as_int{to_int_base(std::string_view{&hex, 1}, 16)};
    return bit_range(as_int, 4);
}

value_t bit_range_to_int(bit_span_t bits)
{
    value_t out{0};
    for (auto b : bits) {
        out <<= 1;
        out |= value_t{b};
    }
    return out;
}

value_t read_int(bit_iter_t& iter, int bit_count)
{
    const value_t out{
        bit_range_to_int({iter, static_cast<std::size_t>(bit_count)})};
    iter += bit_count;
    return out;
}

enum class packet_type_id {
    sum = 0,
    product = 1,
    minimum = 2,
    maximum = 3,
    literal_value = 4,
    less_than = 5,
    greater_than = 6,
    equal_to = 7,
};

std::string to_string(packet_type_id id)
{
    switch (id) {
        case packet_type_id::sum:
            return "sum";
        case packet_type_id::product:
            return "product";
        case packet_type_id::minimum:
            return "minimum";
        case packet_type_id::maximum:
            return "maximum";
        case packet_type_id::literal_value:
            return "literal";
        case packet_type_id::less_than:
            return "less than";
        case packet_type_id::greater_than:
            return "greater than";
        case packet_type_id::equal_to:
            return "equal to";
    }
}

packet_type_id read_packet_type_id(bit_iter_t& iter)
{
    const auto out{static_cast<packet_type_id>(bit_range_to_int({iter, 3}))};
    iter += 3;
    return out;
}

enum class length_type_id {
    length_in_bits = 0,
    number_of_subpackets = 1,
};

class packet_content {
   public:
    virtual ~packet_content() = default;
    virtual std::string to_string() const = 0;
    virtual int version_sum() const = 0;
    virtual value_t value() const = 0;

   protected:
    packet_content() = default;
};

struct packet {
    packet() = default;

    packet(packet&& other)
        : version_{other.version_},
          type_id_{other.type_id_},
          content_{std::move(other.content_)}
    {
    }

    packet& operator=(packet&& other)
    {
        version_ = other.version_;
        type_id_ = other.type_id_;
        content_ = std::move(other.content_);
        return *this;
    }

    int version_sum() const { return version_ + content_->version_sum(); }

    value_t value() const { return content_->value(); }

    int version_;
    packet_type_id type_id_;
    std::unique_ptr<packet_content> content_;
};

std::string packet_string(const packet& p)
{
    return fmt::format("[V{} {}: {}]", p.version_, to_string(p.type_id_),
                       p.content_->to_string());
}

class literal_value : public packet_content {
   public:
    literal_value(value_t value) noexcept : value_{value} {}
    std::string to_string() const override { return fmt::format("{}", value_); }
    int version_sum() const override { return 0; }
    value_t value() const override { return value_; }

   private:
    value_t value_{};
};

class operator_data : public packet_content {
   public:
    operator_data(std::vector<packet>&& subpackets)
        : subpackets_{std::move(subpackets)}
    {
    }

    std::string to_string() const override
    {
        if (subpackets_.empty()) {
            return "[]";
        }
        std::string out{"["};
        out += packet_string(subpackets_[0]);
        for (std::size_t i{1}; i < subpackets_.size(); ++i) {
            out += ", ";
            out += packet_string(subpackets_[i]);
        }
        out += "]";
        return out;
    }

    int version_sum() const override
    {
        int out{0};
        for (const auto& p : subpackets_) {
            out += p.version_sum();
        }
        return out;
    }

   protected:
    std::vector<packet> subpackets_;
};

class sum_operator : public operator_data {
   public:
    sum_operator(std::vector<packet>&& subpackets)
        : operator_data{std::move(subpackets)}
    {
    }

    value_t value() const override
    {
        value_t out{0};
        for (const auto& p : subpackets_) {
            out += p.value();
        }
        return out;
    }
};

class product_operator : public operator_data {
   public:
    product_operator(std::vector<packet>&& subpackets)
        : operator_data{std::move(subpackets)}
    {
    }

    value_t value() const override
    {
        value_t out{1};
        for (const auto& p : subpackets_) {
            out *= p.value();
        }
        return out;
    }
};

class minimum_operator : public operator_data {
   public:
    minimum_operator(std::vector<packet>&& subpackets)
        : operator_data{std::move(subpackets)}
    {
    }

    value_t value() const override
    {
        value_t out{subpackets_[0].value()};
        for (const auto& p : subpackets_) {
            out = std::min(out, p.value());
        }
        return out;
    }
};

class maximum_operator : public operator_data {
   public:
    maximum_operator(std::vector<packet>&& subpackets)
        : operator_data{std::move(subpackets)}
    {
    }

    value_t value() const override
    {
        value_t out{subpackets_[0].value()};
        for (const auto& p : subpackets_) {
            out = std::max(out, p.value());
        }
        return out;
    }
};

class greater_than_operator : public operator_data {
   public:
    greater_than_operator(std::vector<packet>&& subpackets)
        : operator_data{std::move(subpackets)}
    {
    }

    value_t value() const override
    {
        return subpackets_[0].value() > subpackets_[1].value() ? 1 : 0;
    }
};

class less_than_operator : public operator_data {
   public:
    less_than_operator(std::vector<packet>&& subpackets)
        : operator_data{std::move(subpackets)}
    {
    }

    value_t value() const override
    {
        return subpackets_[0].value() < subpackets_[1].value() ? 1 : 0;
    }
};

class equal_to_operator : public operator_data {
   public:
    equal_to_operator(std::vector<packet>&& subpackets)
        : operator_data{std::move(subpackets)}
    {
    }

    value_t value() const override
    {
        return subpackets_[0].value() == subpackets_[1].value() ? 1 : 0;
    }
};

packet read_packet(bit_iter_t& bit_iter, const bit_iter_t end)
{
    if (bit_iter >= end) {
        throw std::runtime_error{"read_packet: ran out of bits"};
    }
    packet p;
    p.version_ = static_cast<int>(read_int(bit_iter, 3));
    p.type_id_ = read_packet_type_id(bit_iter);

    if (p.type_id_ == packet_type_id::literal_value) {
        value_t value{0};
        while (*bit_iter++ == 1) {
            value <<= 4;
            value |= read_int(bit_iter, 4);
        }
        value <<= 4;
        value |= read_int(bit_iter, 4);
        p.content_ = std::make_unique<literal_value>(value);
    }
    else {
        std::vector<packet> subpackets;
        length_type_id length_type{static_cast<length_type_id>(*bit_iter++)};
        switch (length_type) {
            case length_type_id::length_in_bits: {
                int length_in_bits{static_cast<int>(read_int(bit_iter, 15))};
                bit_iter_t target_iter{bit_iter + length_in_bits};
                while (bit_iter < target_iter) {
                    subpackets.emplace_back(read_packet(bit_iter, end));
                }
                break;
            }
            case length_type_id::number_of_subpackets: {
                int number_of_subpackets{
                    static_cast<int>(read_int(bit_iter, 11))};
                for (int i{0}; i < number_of_subpackets; ++i) {
                    subpackets.emplace_back(read_packet(bit_iter, end));
                }
                break;
            }
        }
        switch (p.type_id_) {
            case packet_type_id::sum:
                p.content_ =
                    std::make_unique<sum_operator>(std::move(subpackets));
                break;
            case packet_type_id::product:
                p.content_ =
                    std::make_unique<product_operator>(std::move(subpackets));
                break;
            case packet_type_id::minimum:
                p.content_ =
                    std::make_unique<minimum_operator>(std::move(subpackets));
                break;
            case packet_type_id::maximum:
                p.content_ =
                    std::make_unique<maximum_operator>(std::move(subpackets));
                break;
            case packet_type_id::greater_than:
                p.content_ = std::make_unique<greater_than_operator>(
                    std::move(subpackets));
                break;
            case packet_type_id::less_than:
                p.content_ =
                    std::make_unique<less_than_operator>(std::move(subpackets));
                break;
            case packet_type_id::equal_to:
                p.content_ =
                    std::make_unique<equal_to_operator>(std::move(subpackets));
                break;
            default:
                throw std::runtime_error{"read_packet: unknown packet type"};
        }
    }

    return p;
}

aoc::solution_result day16(std::string_view input)
{
    input = trim(input);
    const auto bits{input | rv::transform(hex_to_bit_range) | rv::join |
                    r::to<std::vector>};
    // TODO: cast_fn
    auto as_string{bits |
                   rv::transform([](bit_t bit) { return bit ? '1' : '0'; }) |
                   r::to<std::string>};

    auto bit_iter{bits.begin()};

    packet p{read_packet(bit_iter, bits.end())};

    fmt::print("{}\n", packet_string(p));

    return {p.version_sum(), p.value()};
}

}  // namespace aoc::year2021
