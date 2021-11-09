#include <greeting.hpp>

#include <catch2/catch.hpp>

#include <sstream>

namespace {
auto to_string(const auto& printable) -> std::string
{
    std::ostringstream text;
    text << printable;
    return text.str();
}
}  // namespace

TEST_CASE("Greetings are printed correctly", "[greeting]")
{
    CHECK(to_string(hello::greeting{}) == "Hello!");
    CHECK(to_string(hello::greeting{"World"}) == "Hello World!");
    CHECK(to_string(hello::greeting{"David"}) == "Hello David!");
}
