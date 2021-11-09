add_executable(tests tests.cpp hello_tests.cpp)
target_link_libraries(tests PRIVATE project_options project_warnings Catch2::Catch2 hello)

catch_discover_tests(tests)
