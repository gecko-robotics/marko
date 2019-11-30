
add_executable(request request.cpp)
target_link_libraries(request marko)
# target_include_directories(request PUBLIC Marko::Marko)

# add_executable(reply reply.cpp)
# target_link_libraries(reply Marko::marko)
# # target_include_directories(reply PUBLIC Marko::Marko)
#
# add_executable(listen listen.cpp)
# target_link_libraries(listen Marko::marko)
# # target_include_directories(listen PUBLIC Marko::Marko)
