//
// Copyright (c) 2022 David Holmes (dholmes at dholmes dot us)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <tiny_vector.hpp>

#include <catch2/catch_all.hpp>

#include <algorithm>
#include <array>
#include <string_view>

using namespace aoc;

// TODO: make some stuff in this file const where possible
// TODO: test both const and non-const versions of functions

TEST_CASE("tiny_vector default constructor/destructor", "[tiny_vector]")
{
    {
        tiny_vector<char> v_char;
    }
    {
        enum class test_enum : char {};
        tiny_vector<test_enum> v_enum;
    }
}

TEST_CASE("tiny_vector count constructors", "[tiny_vector]")
{
    tiny_vector<char> v(5, 'x');
    CHECK(v.size() == 5);
    CHECK(v[0] == 'x');
    CHECK(v[4] == 'x');

    struct test_object {
        char c{'y'};
    };

    tiny_vector<test_object> v2(5);
    CHECK(v2.size() == 5);
    CHECK(v2[0].c == 'y');
    CHECK(v2[4].c == 'y');
}

TEST_CASE("tiny_vector range constructor", "[tiny_vector]")
{
    const std::string_view hello{"Hello"};
    tiny_vector<char> v1{hello.begin(), hello.end()};
    CHECK(v1.size() == 5);
    const std::string_view v1_sv{v1.begin(), v1.end()};
    CHECK(v1_sv == hello);

    const std::string_view hello_world{"Hello World"};
    REQUIRE_THROWS_AS(
        (tiny_vector<char>{hello_world.begin(), hello_world.end()}),
        tiny_vector<char>::capacity_error);
}

// Movable type to demonstrate whether copy or move functions are called
// correctly
struct movable_char {
    char c;

    movable_char() noexcept = default;
    movable_char(const char input) : c{input} {};
    movable_char(const movable_char& other) noexcept { c = other.c; }
    movable_char(movable_char&& other) noexcept
    {
        c = other.c;
        other.c = '*';
    }
    movable_char& operator=(const movable_char& other) noexcept
    {
        c = other.c;
        return *this;
    }
    movable_char& operator=(movable_char&& other) noexcept
    {
        c = other.c;
        other.c = '*';
        return *this;
    }

    friend bool operator==(const movable_char&,
                           const movable_char&) noexcept = default;
};

// Movable type to demonstrate whether copy or move functions are called
// correctly
struct copy_throwing_char {
    char c;

    copy_throwing_char() noexcept = default;
    copy_throwing_char(const char input) : c{input} {};
    copy_throwing_char(const copy_throwing_char&)
    {
        throw std::runtime_error("copy was attempted");
    }
    copy_throwing_char(copy_throwing_char&& other) noexcept
    {
        c = other.c;
        other.c = '*';
    }
    copy_throwing_char& operator=(const copy_throwing_char&)
    {
        throw std::runtime_error("copy was attempted");
    }
    copy_throwing_char& operator=(copy_throwing_char&& other) noexcept
    {
        c = other.c;
        other.c = '*';
        return *this;
    }

    friend bool operator==(const copy_throwing_char&,
                           const copy_throwing_char&) noexcept = default;
};

TEST_CASE("tiny_vector copy constructor", "[tiny_vector]")
{
    const std::string_view hello{"Hello"};
    tiny_vector<char> v1{hello.begin(), hello.end()};
    CHECK(v1.size() == 5);

    const tiny_vector<char> v2{v1};
    CHECK(v2.size() == 5);

    const std::string_view v2_sv{v2.begin(), v2.end()};
    CHECK(v2_sv == hello);
}

TEST_CASE("tiny_vector copy constructor copies", "[tiny_vector]")
{
    const std::array<movable_char, 5> hello{'h', 'e', 'l', 'l', 'o'};

    tiny_vector<movable_char> v1{hello.begin(), hello.end()};
    tiny_vector<movable_char> v2{v1};
    CHECK(v1.size() == 5);
    CHECK(v2.size() == 5);
    CHECK(std::ranges::equal(hello, v1));
    CHECK(std::ranges::equal(v1, v2));
}

TEST_CASE("tiny_vector move constructor moves", "[tiny_vector]")
{
    tiny_vector<copy_throwing_char> v1(5);
    v1[0].c = 'h';
    v1[1].c = 'e';
    v1[2].c = 'l';
    v1[3].c = 'l';
    v1[4].c = 'o';
    tiny_vector<copy_throwing_char> v2{std::move(v1)};
    CHECK(v1.size() == 0);
    CHECK(v2.size() == 5);
    CHECK(v2[0].c == 'h');
    CHECK(v2[1].c == 'e');
    CHECK(v2[2].c == 'l');
    CHECK(v2[3].c == 'l');
    CHECK(v2[4].c == 'o');
}

TEST_CASE("tiny_vector initializer list constructor", "[tiny_vector]")
{
    const std::string_view hello{"hello"};
    tiny_vector<char> v{'h', 'e', 'l', 'l', 'o'};
    CHECK(v.size() == 5);
    CHECK(std::ranges::equal(hello, v));

    REQUIRE_THROWS_AS((tiny_vector<char>{'h', 'e', 'l', 'l', 'o', ' ', 'W', 'o',
                                         'r', 'l', 'd'}),
                      tiny_vector<char>::capacity_error);
}

TEST_CASE("tiny_vector copy assignment copies", "[tiny_vector]")
{
    const std::array<movable_char, 5> hello{'h', 'e', 'l', 'l', 'o'};

    tiny_vector<movable_char> v1{hello.begin(), hello.end()};
    tiny_vector<movable_char> v2;
    v2 = v1;
    CHECK(v1.size() == 5);
    CHECK(v2.size() == 5);
    CHECK(std::ranges::equal(hello, v1));
    CHECK(std::ranges::equal(v1, v2));
}

TEST_CASE("tiny_vector move assignment moves", "[tiny_vector]")
{
    tiny_vector<copy_throwing_char> v1(5);
    v1[0].c = 'h';
    v1[1].c = 'e';
    v1[2].c = 'l';
    v1[3].c = 'l';
    v1[4].c = 'o';
    tiny_vector<copy_throwing_char> v2;
    v2 = std::move(v1);
    CHECK(v1.size() == 0);
    CHECK(v2.size() == 5);
    CHECK(v2[0].c == 'h');
    CHECK(v2[1].c == 'e');
    CHECK(v2[2].c == 'l');
    CHECK(v2[3].c == 'l');
    CHECK(v2[4].c == 'o');
}

TEST_CASE("tiny_vector copy push_back", "[tiny_vector]")
{
    const std::array<movable_char, 11> hello{'h', 'e', 'l', 'l', 'o', ' ',
                                             'W', 'o', 'r', 'l', 'd'};
    std::array<movable_char, 11> input{hello};
    const movable_char asterisk{'*'};

    tiny_vector<movable_char> v;

    CHECK(v.size() == 0);
    v.push_back(input[0]);
    CHECK(v[0] == hello[0]);
    CHECK(input[0] == hello[0]);
    CHECK(v.size() == 1);

    v.push_back(input[1]);
    CHECK(v[1] == hello[1]);
    CHECK(input[1] == hello[1]);
    CHECK(v.size() == 2);

    v.push_back(input[2]);
    CHECK(v[2] == hello[2]);
    CHECK(input[2] == hello[2]);
    CHECK(v.size() == 3);

    v.push_back(input[3]);
    CHECK(v[3] == hello[3]);
    CHECK(input[3] == hello[3]);
    CHECK(v.size() == 4);

    v.push_back(input[4]);
    CHECK(v[4] == hello[4]);
    CHECK(input[4] == hello[4]);
    CHECK(v.size() == 5);

    v.push_back(input[5]);
    CHECK(v[5] == hello[5]);
    CHECK(input[5] == hello[5]);
    CHECK(v.size() == 6);

    v.push_back(input[6]);
    CHECK(v[6] == hello[6]);
    CHECK(input[6] == hello[6]);
    CHECK(v.size() == 7);

    // push_back beyond capacity throws and does not move
    REQUIRE_THROWS_AS(v.push_back(input[7]),
                      tiny_vector<movable_char>::capacity_error);
    CHECK(input[7] == hello[7]);
    CHECK(v.size() == 7);
}

TEST_CASE("tiny_vector move push_back", "[tiny_vector]")
{
    const std::array<movable_char, 11> hello{'h', 'e', 'l', 'l', 'o', ' ',
                                             'W', 'o', 'r', 'l', 'd'};
    std::array<movable_char, 11> input{hello};
    const movable_char asterisk{'*'};

    tiny_vector<movable_char> v;

    CHECK(v.size() == 0);
    v.push_back(std::move(input[0]));
    CHECK(v[0] == hello[0]);
    CHECK(input[0] == asterisk);
    CHECK(v.size() == 1);

    v.push_back(std::move(input[1]));
    CHECK(v[1] == hello[1]);
    CHECK(input[1] == asterisk);
    CHECK(v.size() == 2);

    v.push_back(std::move(input[2]));
    CHECK(v[2] == hello[2]);
    CHECK(input[2] == asterisk);
    CHECK(v.size() == 3);

    v.push_back(std::move(input[3]));
    CHECK(v[3] == hello[3]);
    CHECK(input[3] == asterisk);
    CHECK(v.size() == 4);

    v.push_back(std::move(input[4]));
    CHECK(v[4] == hello[4]);
    CHECK(input[4] == asterisk);
    CHECK(v.size() == 5);

    v.push_back(std::move(input[5]));
    CHECK(v[5] == hello[5]);
    CHECK(input[5] == asterisk);
    CHECK(v.size() == 6);

    v.push_back(std::move(input[6]));
    CHECK(v[6] == hello[6]);
    CHECK(input[6] == asterisk);
    CHECK(v.size() == 7);

    // push_back beyond capacity throws and does not move
    REQUIRE_THROWS_AS(v.push_back(std::move(input[7])),
                      tiny_vector<movable_char>::capacity_error);
    CHECK(input[7] == hello[7]);
    CHECK(v.size() == 7);
}

TEST_CASE("tiny_vector pop_back", "[tiny_vector]")
{
    tiny_vector<char> v{'h', 'e', 'l', 'l', 'o'};
    v.pop_back();
    CHECK(v == tiny_vector<char>{'h', 'e', 'l', 'l'});
    v.pop_back();
    CHECK(v == tiny_vector<char>{'h', 'e', 'l'});
    v.pop_back();
    CHECK(v == tiny_vector<char>{'h', 'e'});
    v.pop_back();
    CHECK(v == tiny_vector<char>{'h'});
    v.pop_back();
    CHECK(v.empty());
}

TEST_CASE("tiny_vector operator==", "[tiny_vector]")
{
    tiny_vector<char> hello1{'h', 'e', 'l', 'l', 'o'};
    tiny_vector<char> hello2{'h', 'e', 'l', 'l', 'o'};
    tiny_vector<char> hell{'h', 'e', 'l', 'l'};
    CHECK(hello1 == hello2);
    CHECK(hello1 != hell);
}