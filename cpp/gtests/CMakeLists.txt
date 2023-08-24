# # GTest -----------------
# include(FetchContent)
# # FetchContent_Declare(gtest
# #   GIT_REPOSITORY "https://github.com/google/googletest"
# #   GIT_TAG "origin/main"
# #   SOURCE_DIR "${CMAKE_BINARY_DIR}/_deps/gtest"
# # )
# FetchContent_Declare(
#     gtest
#     URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
# )

# FetchContent_MakeAvailable(gtest)
# if(gtest_POPULATED)
#     message(STATUS "=> Found gtest")
# else()
#     message(STATUS "*** Didn't find gtest")
# endif()

enable_testing()

LIST(APPEND gtests
    test_bc.cpp
    test_ps.cpp
    test_rr.cpp
    test_marko.cpp
)

# run all tests
add_executable(test-all ${gtests})
target_link_libraries(test-all GTest::gtest_main ${PROJECT_NAME})
# add_test(NAME test-all COMMAND test-all)
include(GoogleTest)
gtest_discover_tests(test-all)

# foreach(MAIN ${gtests})
#     add_executable(${MAIN} ${MAIN}.cpp)
#     target_link_libraries(${MAIN} PRIVATE
#         marko
#         GTest::gtest_main)
# endforeach()

# include(GoogleTest)
# gtest_discover_tests(test_marko)