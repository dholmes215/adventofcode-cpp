add_library(aoc_lib aoc.cpp aoc.hpp)
target_include_directories(aoc_lib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(aoc_lib PUBLIC project_options fmt::fmt
                              PRIVATE project_warnings tl::expected range-v3::range-v3)
