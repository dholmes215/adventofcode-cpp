add_executable(tests tests.cpp aoctests.cpp aoc_grid_tests.cpp aoc_range_tests.cpp aoc_vec_tests.cpp year2015tests.cpp)
target_link_libraries(tests PRIVATE project_options project_warnings Catch2::Catch2 aoc2015 md5 aoc_lib)

catch_discover_tests(tests)
