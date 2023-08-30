
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
add_test(NAME test-all COMMAND test-all)
include(GoogleTest)
gtest_discover_tests(test-all)