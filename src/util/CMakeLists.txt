find_package(CMakeRC)

cmrc_add_resource_library(aoc_resources ALIAS aoc::rc NAMESPACE aoc template_CMakeLists.txt template_year.hpp.txt template_day.cpp.txt)

add_executable(generate_year generate_year.cpp)
target_link_libraries(generate_year PUBLIC project_options
                                    PRIVATE project_warnings aoc_lib aoc::rc)
