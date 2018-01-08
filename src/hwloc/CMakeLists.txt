
set (INCLUDE_DIR /usr/local/include)
set(LIB_DIR /usr/local/lib)


include_directories(${INCLUDE_DIR})
link_directories(${LIB_DIR})

add_executable(hwloc main.cpp)
add_executable(gpu get-knl-modes.c)


target_link_libraries(hwloc
        -lhwloc
        )

target_link_libraries(gpu
        -lhwloc
        )