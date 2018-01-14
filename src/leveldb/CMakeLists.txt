add_executable(ldb ldb.cpp)
add_executable(ldb_test ldb_test.cpp)

include_directories(/usr/include/leveldb
        )
link_directories(/usr/local/lib
        )
target_link_libraries(ldb leveldb)
target_link_libraries(ldb_test leveldb)