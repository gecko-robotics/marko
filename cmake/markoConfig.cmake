set(marko_FOUND 1)
set(marko_VERSION "0.0.1")
set(marko_INCLUDE_DIRS /opt/gecko/include)

if(APPLE)
    set(marko_LIBRARIES /opt/gecko/lib/libmarko.dylib)
elseif(UNIX AND NOT APPLE)
    set(marko_LIBRARIES /opt/gecko/lib/libmarko.so)
endif()

message(STATUS "===============================================")
message(STATUS " Marko ${marko_VERSION}")
message(STATUS " Exporting marko_INCLUDE_DIRS marko_LIBRARIES")
