#ifndef GREETING_HPP
#define GREETING_HPP

#include <iostream>
#include <string>

namespace hello {

struct greeting {
    std::string name{};
    friend auto operator<<(std::ostream& out, const greeting& obj) -> std::ostream&;
};

}  // namespace hello

#endif  // GREETING_HPP
