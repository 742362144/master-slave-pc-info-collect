

add_executable(main main.cpp PCInfoTools.h PCInfoTools.cpp)
set(LIBTOOLS_SRC PCInfoTools.cpp)
ADD_LIBRARY(tools SHARED ${LIBTOOLS_SRC})