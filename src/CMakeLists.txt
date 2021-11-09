add_library(hello greeting.cpp greeting.hpp)
target_include_directories(hello INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(hello PUBLIC project_options 
                            PRIVATE project_warnings)

add_executable(hello_app main.cpp)
target_link_libraries(hello_app PRIVATE project_options project_warnings hello fmt::fmt)
