
add_executable(request request.cpp)
target_link_libraries(request marko)

add_executable(reply reply.cpp)
target_link_libraries(reply marko)

add_executable(listen listen.cpp)
target_link_libraries(listen marko)
