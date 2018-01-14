

set (INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src
        /usr/local/include
        /usr/local/brpc/output/include)
set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib
        /usr/local/lib
        /usr/local/brpc/output/lib)


include_directories(${INCLUDE_DIR})
link_directories(${LIB_DIR})

add_subdirectory(proto)
add_subdirectory(tools)
add_subdirectory(sqlite)
add_subdirectory(hwloc)
add_subdirectory(leveldb)

add_executable(slave slave.cpp)
add_executable(master master.cpp)


target_link_libraries(slave
        -lgflags
        -lbrpc
        -lprotobuf
        -lleveldb
        proto
        tools
        )

target_link_libraries(master
        -lgflags
        -lbrpc
        -lprotobuf
        -lleveldb
        proto
        tools
        )
set_target_properties(slave PROPERTIES LINK_FLAGS -Wl,-rpath=/usr/local/lib)

