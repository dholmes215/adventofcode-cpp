add_executable(tests aoctests.cpp aoc_graph_tests.cpp aoc_grid_tests.cpp aoc_range_tests.cpp aoc_vec_tests.cpp year2015tests.cpp year2021tests.cpp tiny_vector_tests.cpp)
target_link_libraries(tests PRIVATE project_options project_warnings Catch2::Catch2WithMain aoc2015 aoc2021 md5 aoc_lib)

catch_discover_tests(tests)
