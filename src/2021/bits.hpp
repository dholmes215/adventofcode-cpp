//
// Copyright (c) 2020-2021 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BITS_HPP
#define BITS_HPP

#include <aoc.hpp>
#include <aoc_range.hpp>

#include <fmt/format.h>

#include <cstdint>
#include <memory>
#include <span>
#include <stdexcept>
#include <string_view>
#include <vector>

namespace aoc::year2021 {

using bit_t = std::uint8_t;
using bit_range_t = r::any_view<bit_t>;
using bit_iter_t = std::vector<bit_t>::const_iterator;
using bit_span_t = std::span<const bit_t>;
using value_t = std::uint64_t;

enum class packet_type_id {
    sum = 0,
    product = 1,
    minimum = 2,
    maximum = 3,
    literal = 4,
    greater_than = 5,
    less_than = 6,
    equal_to = 7,
};

enum class length_type_id {
    length_in_bits = 0,
    number_of_subpackets = 1,
};

decltype(bit_range<bit_t>(0, 4)) hex_to_bit_range(char hex);

std::vector<bit_t> hex_to_bit_vector(std::string_view hex);

value_t bit_range_to_int(bit_span_t bits);

value_t read_int(bit_iter_t& iter, int bit_count);

std::string to_string(packet_type_id id);

packet_type_id read_packet_type_id(bit_iter_t& iter);

class packet_content {
   public:
    virtual ~packet_content() = default;
    virtual std::string to_string() const = 0;
    virtual int version_sum() const = 0;
    virtual value_t value() const = 0;
    virtual void print_tree(int indent) const = 0;

   protected:
    packet_content() = default;
};

struct packet {
    packet() = default;
    packet(packet&& other);
    packet& operator=(packet&& other);
    int version_sum() const { return version_ + content_->version_sum(); }
    value_t value() const { return content_->value(); }
    void print_tree(int indent = 0) const;

    int version_;
    packet_type_id type_id_;
    std::unique_ptr<packet_content> content_;
};

class literal : public packet_content {
   public:
    literal(value_t value) noexcept : value_{value} {}
    std::string to_string() const override { return fmt::format("{}", value_); }
    int version_sum() const override { return 0; }
    value_t value() const override { return value_; }
    void print_tree(int indent) const override { (void)indent; }

   private:
    value_t value_{};
};

class operator_data : public packet_content {
   public:
    operator_data(std::vector<packet>&& subpackets);
    std::string to_string() const override;
    int version_sum() const override;
    void print_tree(int indent) const override;

   protected:
    std::vector<packet> subpackets_;
};

class sum_operator : public operator_data {
   public:
    sum_operator(std::vector<packet>&& subpackets);
    value_t value() const override;
};

class product_operator : public operator_data {
   public:
    product_operator(std::vector<packet>&& subpackets);
    value_t value() const override;
};

class minimum_operator : public operator_data {
   public:
    minimum_operator(std::vector<packet>&& subpackets);
    value_t value() const override;
};

class maximum_operator : public operator_data {
   public:
    maximum_operator(std::vector<packet>&& subpackets);
    value_t value() const override;
};

class greater_than_operator : public operator_data {
   public:
    greater_than_operator(std::vector<packet>&& subpackets);
    value_t value() const override;
};

class less_than_operator : public operator_data {
   public:
    less_than_operator(std::vector<packet>&& subpackets);
    value_t value() const override;
};

class equal_to_operator : public operator_data {
   public:
    equal_to_operator(std::vector<packet>&& subpackets);
    value_t value() const override;
};

std::string packet_string(const packet& p);

value_t read_literal(bit_iter_t& bit_iter);

packet read_packet(bit_iter_t& bit_iter, const bit_iter_t end);

}  // namespace aoc::year2021

#endif  // BITS_HPP
