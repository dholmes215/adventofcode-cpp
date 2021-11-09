#include "greeting.hpp"

namespace hello {

auto operator<<(std::ostream& out, const greeting& obj) -> std::ostream& {
    if (obj.name.empty()) {
        return out << "Hello!";
    }
    return out << "Hello " << obj.name << '!';
}

}  // namespace hello
