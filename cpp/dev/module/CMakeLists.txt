cmake_minimum_required(VERSION 3.16.0)
project(hello VERSION 0.5.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(PREBUILT_MODULE_PATH ${CMAKE_BINARY_DIR}/modules)

function(add_module name)
    file(MAKE_DIRECTORY ${PREBUILT_MODULE_PATH})
    add_custom_target(${name}.pcm
            COMMAND
                # ${CMAKE_CXX_COMPILER}
                clang++
                -std=c++20
                -fmodules-ts
                -stdlib=libc++
                -fmodules
                -fbuiltin-module-map
                -c
                ${CMAKE_CURRENT_SOURCE_DIR}/${ARGN}
                -Xclang -emit-module-interface
                -o ${PREBUILT_MODULE_PATH}/${name}.pcm

            )
endfunction()


add_compile_options(-fmodules)
add_compile_options(-std=c++20)
add_compile_options(-stdlib=libc++)
add_compile_options(-fmodules-ts)
add_compile_options(-fbuiltin-module-map)
add_compile_options(-fimplicit-module-maps)
add_compile_options(-fprebuilt-module-path=${PREBUILT_MODULE_PATH})

add_module(helloworld helloworld.cpp)
add_executable(main
        main.cpp
        # helloworld.cpp
        )
add_dependencies(main helloworld.pcm)
