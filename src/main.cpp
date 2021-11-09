#include "greeting.hpp"

#include <fmt/format.h>
#include <fmt/ostream.h>

int main()
{
    fmt::print("{}\n", hello::greeting{"World"});
}
