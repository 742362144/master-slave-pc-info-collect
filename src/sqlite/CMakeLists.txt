add_executable(new new.cpp)
include_directories(/usr/lib64/perl5/vendor_perl/auto/share/dist/DBD-SQLite
        )
link_directories(/usr/local/lib
        )
target_link_libraries(new sqlite3)