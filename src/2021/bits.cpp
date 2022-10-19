//
// Copyright (c) 2020-2021 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "bits.hpp"

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

decltype(bit_range<bit_t>(0, 4)) hex_to_bit_range(char hex)
{
    const int as_int{to_int_base(std::string_view{&hex, 1}, 16)};
    return bit_range<bit_t>(as_int, 4);
}

std::vector<bit_t> hex_to_bit_vector(std::string_view hex)
{
    return trim(hex) | rv::transform(hex_to_bit_range) | rv::join |
           r::to<std::vector>;
}

value_t bit_range_to_int(bit_span_t bits)
{
    if (bits.size() == 0) {
        throw std::runtime_error{"bit_range_to_int: empty bit range"};
    }
    if (bits.size() > 64) {
        throw std::runtime_error{"bit_range_to_int: too many bits"};
    }
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
        case packet_type_id::literal:
            return "literal";
        case packet_type_id::greater_than:
            return "greater than";
        case packet_type_id::less_than:
            return "less than";
        case packet_type_id::equal_to:
            return "equal to";
    }
    throw input_error(
        fmt::format("Invalid packet type: {}", static_cast<int>(id)));
}

packet_type_id read_packet_type_id(bit_iter_t& iter)
{
    const auto out{static_cast<packet_type_id>(bit_range_to_int({iter, 3}))};
    iter += 3;
    return out;
}

packet::packet(packet&& other)
    : version_{other.version_},
      type_id_{other.type_id_},
      content_{std::move(other.content_)}
{
}

packet& packet::operator=(packet&& other)
{
    version_ = other.version_;
    type_id_ = other.type_id_;
    content_ = std::move(other.content_);
    return *this;
}

void packet::print_tree(int indent) const
{
    for (int i{0}; i < indent; ++i) {
        fmt::print("  ");
    }
    fmt::print("{}: {}\n", value(), to_string(type_id_));
    content_->print_tree(indent + 1);
}

operator_data::operator_data(std::vector<packet>&& subpackets)
    : subpackets_{std::move(subpackets)}
{
}

std::string operator_data::to_string() const
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

int operator_data::version_sum() const
{
    int out{0};
    for (const auto& p : subpackets_) {
        out += p.version_sum();
    }
    return out;
}

void operator_data::print_tree(int indent) const
{
    for (const auto& p : subpackets_) {
        p.print_tree(indent);
    }
}

sum_operator::sum_operator(std::vector<packet>&& subpackets)
    : operator_data{std::move(subpackets)}
{
}

value_t sum_operator::value() const
{
    value_t out{0};
    for (const auto& p : subpackets_) {
        out += p.value();
    }
    return out;
}

product_operator::product_operator(std::vector<packet>&& subpackets)
    : operator_data{std::move(subpackets)}
{
}

value_t product_operator::value() const
{
    value_t out{1};
    for (const auto& p : subpackets_) {
        out *= p.value();
    }
    return out;
}

minimum_operator::minimum_operator(std::vector<packet>&& subpackets)
    : operator_data{std::move(subpackets)}
{
}

value_t minimum_operator::value() const
{
    value_t out{subpackets_[0].value()};
    for (const auto& p : subpackets_) {
        out = std::min(out, p.value());
    }
    return out;
}

maximum_operator::maximum_operator(std::vector<packet>&& subpackets)
    : operator_data{std::move(subpackets)}
{
}

value_t maximum_operator::value() const
{
    value_t out{subpackets_[0].value()};
    for (const auto& p : subpackets_) {
        out = std::max(out, p.value());
    }
    return out;
}

greater_than_operator::greater_than_operator(std::vector<packet>&& subpackets)
    : operator_data{std::move(subpackets)}
{
    if (subpackets_.size() != 2) {
        throw std::runtime_error(
            "greater_than_operator must have exactly 2 subpackets");
    }
}

value_t greater_than_operator::value() const
{
    return subpackets_[0].value() > subpackets_[1].value() ? 1 : 0;
}

less_than_operator::less_than_operator(std::vector<packet>&& subpackets)
    : operator_data{std::move(subpackets)}
{
    if (subpackets_.size() != 2) {
        throw std::runtime_error(
            "less_than_operator must have exactly 2 "
            "subpackets");
    }
}

value_t less_than_operator::value() const
{
    return subpackets_[0].value() < subpackets_[1].value() ? 1 : 0;
}

equal_to_operator::equal_to_operator(std::vector<packet>&& subpackets)
    : operator_data{std::move(subpackets)}
{
    if (subpackets_.size() != 2) {
        throw std::runtime_error(
            "equal_to_operator must have exactly 2 "
            "subpackets");
    }
}

value_t equal_to_operator::value() const
{
    return subpackets_[0].value() == subpackets_[1].value() ? 1 : 0;
}

std::string packet_string(const packet& p)
{
    if (p.type_id_ == packet_type_id::literal) {
        return fmt::format("{}", p.value());
    }
    return fmt::format("[{}: {}]", to_string(p.type_id_),
                       p.content_->to_string());
}

value_t read_literal(bit_iter_t& bit_iter)
{
    const bit_iter_t iter_begin{bit_iter};
    value_t value{0};
    while (*bit_iter++ == 1) {
        value <<= 4;
        value |= read_int(bit_iter, 4);
    }
    value <<= 4;
    value |= read_int(bit_iter, 4);

    if (r::distance(iter_begin, bit_iter) > 80) {
        throw std::runtime_error("literal input must be no more than 80 bits");
    }

    if (r::distance(iter_begin, bit_iter) % 5 != 0) {
        throw std::logic_error("literal input must be multiple of 5");
    }

    return value;
}

packet read_packet(bit_iter_t& bit_iter, const bit_iter_t end)
{
    if (bit_iter >= end) {
        throw std::runtime_error{"read_packet: ran out of bits"};
    }
    packet p;
    p.version_ = static_cast<int>(read_int(bit_iter, 3));
    p.type_id_ = read_packet_type_id(bit_iter);

    if (p.type_id_ == packet_type_id::literal) {
        p.content_ = std::make_unique<literal>(read_literal(bit_iter));
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
                if (bit_iter != target_iter) {
                    throw std::runtime_error{
                        "read_packet: length_in_bits "
                        "did not match length of packets read"};
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

}  // namespace aoc::year2021
