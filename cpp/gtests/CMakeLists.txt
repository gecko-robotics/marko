# GTest -----------------
include(FetchContent)
FetchContent_Declare(gtest
  GIT_REPOSITORY "https://github.com/google/googletest"
  GIT_TAG "origin/main"
  SOURCE_DIR "${CMAKE_BINARY_DIR}/_deps/gtest"
)
# set(EXAMPLES OFF CACHE INTERNAL "Dont build examples")
FetchContent_MakeAvailable(gtest)
if(gtest_POPULATED)
    message(STATUS "=> Found gtest")
else()
    message(STATUS "*** Didn't find gtest")
endif()

enable_testing()

LIST(APPEND gtests
    test_bc
    test_ps
    test_rr
    test_marko
)

foreach(MAIN ${gtests})
    add_executable(${MAIN} ${MAIN}.cpp)
    target_link_libraries(${MAIN} PRIVATE
        marko
        GTest::gtest_main)
endforeach()

include(GoogleTest)
# gtest_discover_tests(test_marko)