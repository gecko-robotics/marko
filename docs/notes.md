[Beej's Guide to Netowrking](https://beej.us/guide/bgnet/html//index.html#structs)

```c
// sa_data contains a destination address and port number for the socket.
// This is rather unwieldy since you don’t want to tediously pack the address
// in the sa_data by hand.
struct sockaddr {
    unsigned short    sa_family;    // address family, AF_xxx
    char              sa_data[14];  // 14 bytes of protocol address
};

// To deal with struct sockaddr, programmers created a parallel
// structure: struct sockaddr_in (“in” for “Internet”) to be used with IPv4.
struct sockaddr_in {
    short int          sin_family;  // Address family, AF_INET
    unsigned short int sin_port;    // Port number
    struct in_addr     sin_addr;    // Internet address
    unsigned char      sin_zero[8]; // Same size as struct sockaddr
};

struct in_addr {
    uint32_t s_addr; // that's a 32-bit int (4 bytes)
};
```
```c
struct sockaddr_in sa; // IPv4

// See, inet_pton() returns -1 on error, or 0 if the address is messed up.
inet_pton(AF_INET, "10.12.110.57", &(sa.sin_addr)); // IPv4

char ip4[INET_ADDRSTRLEN];  // space to hold the IPv4 string

// “ntop” means “network to presentation”—you can call it
// “network to printable” if that’s easier to remember
inet_ntop(AF_INET, &(sa.sin_addr), ip4, INET_ADDRSTRLEN);
printf("The IPv4 address is: %s\n", ip4);
```

### Reusable

```c
int on = 1;
setsockopt(sock, SOL_SOCKET,  SO_REUSEADDR, (char *)&on, sizeof(on));
```

### Timeout

I think I like timeout better than nonblocking or `select`. `select` has to
be done everytime you read a socket and nonblocking raises signals that need
to be caught.

```c
struct timeval tv;
long sec = 0;
long msec = 0;

int timeout = 5000; // msec

if (timeout >= 1000) {
    sec = (long)timeout/1000;
    timeout %= 1000;
}

tv.tv_sec = sec;
tv.tv_usec = msec * 1000;

err = ::setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
```

### Nonblocking

```c
int on = 1;
int err = 0;

// this appears unnecessary ... it will enable nonblocking, but you still
// need select to set timeout
err = ioctl(sock, FIONBIO, (char *)&on);

// set timeout for 100 ms
timeout.tv_sec  = 0;
timeout.tv_usec = 100 * 1000; // this is usec (1000usec = 1msec)

fd_set readfds;         // read file descriptor
FD_ZERO(&readfds);      // clear
FD_SET(sock, &readfds); // puts sock into fd_set

// if err is 0, then nothing happened
err = select(sock + 1, &working_set, NULL, NULL, &timeout);

// unnecessary, but you can check to see if data there ... err above already
// tells you that.
if (FD_ISSET(sock, &readfds) == 0) { /* no data */ }
```

> On success, `select()` and `pselect()` return the number of file
descriptors contained in the three returned descriptor sets (that
is, the total number of bits that are set in readfds, writefds,
exceptfds).  **The return value may be zero if the timeout expired
before any file descriptors became ready.**
>
> On error, -1 is returned, and errno is set to indicate the error;
> the file descriptor sets are unmodified, and timeout becomes
> undefined.
>
> [ref](https://man7.org/linux/man-pages/man2/select.2.html)

# CMake

Setup your source like:

```
main.cpp
CMakeTexts.txt
```

To pull this library from github and use it in a program, set your `CMakeTexts.txt` up like:

```cmake
cmake_minimum_required(VERSION 3.10.0)
project(ps VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

cmake_host_system_information(RESULT HOST QUERY HOSTNAME)
cmake_host_system_information(RESULT OSN QUERY OS_NAME)
cmake_host_system_information(RESULT OS_VERSION QUERY OS_RELEASE)
cmake_host_system_information(RESULT PROC QUERY PROCESSOR_DESCRIPTION)

message(STATUS "-------------------------------------")
message(STATUS "  Project: ${PROJECT_NAME}")
message(STATUS "  C++ ${CMAKE_CXX_STANDARD}")
message(STATUS "-------------------------------------")
message(STATUS " ${HOST}")
message(STATUS " ${OSN}: ${OS_VERSION}")
message(STATUS " ${PROC}")
message(STATUS "-------------------------------------")


# Library =====================================================================

include(ExternalProject)
ExternalProject_Add(marko-proj
  GIT_REPOSITORY    https://github.com/gecko-robotics/marko.cpp.git
  GIT_TAG           main
  SOURCE_DIR        "${CMAKE_CURRENT_BINARY_DIR}/marko/src"
  BINARY_DIR        "${CMAKE_CURRENT_BINARY_DIR}/marko"
  # CONFIGURE_COMMAND ""
  BUILD_COMMAND     "make"
  INSTALL_COMMAND   ""
  # TEST_COMMAND      ""
)

SET(MARKO_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/marko/src/include)
SET(MARKO_LIB_DIR ${CMAKE_CURRENT_BINARY_DIR}/marko)

# Binary ======================================================================
add_executable(${PROJECT_NAME} ps.cpp)
add_dependencies(${PROJECT_NAME} marko-proj)
target_include_directories(${PROJECT_NAME} PRIVATE ${MARKO_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE marko)
target_link_directories(${PROJECT_NAME} PRIVATE ${MARKO_LIB_DIR})
```

Now do:

```
mkdir build
cd build
cmake ..
make
```
