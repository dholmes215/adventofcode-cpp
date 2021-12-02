add_library(aoc_lib aoc.cpp aoc.hpp aoc_enum.hpp aoc_grid.hpp aoc_range.hpp aoc_vec.hpp)
target_include_directories(aoc_lib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(aoc_lib PUBLIC project_options fmt::fmt range-v3::range-v3
                              PRIVATE project_warnings tl::expected)
