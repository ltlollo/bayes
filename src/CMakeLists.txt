set(CXX_FLAGS
    "-Wall"
    "-Wextra"
    "-Werror"
    "-std=c++14"
    "-pedantic"
    "-fno-rtti"
    "-fPIE")

foreach(FLAG ${CXX_FLAGS})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${FLAG}")
endforeach()

set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")
set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -DNDEBUG -march=native ${LTO_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -march=native ${LTO_FLAGS}")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g ${LTO_FLAGS}")

set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--no-undefined,-02 ${LTO_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${LTO_FLAGS}")
set(CMAKE_MODULE_LINKER_FLAGS "${LTO_FLAGS}")

find_package(Extra REQUIRED)

add_custom_target(hack SOURCES bayes_impl.h)

add_library(libbayes bayes.cpp)
add_executable(bserver server.cpp)
add_executable(bclient client.cpp)
target_link_libraries(bserver libbayes)
target_link_libraries(bserver ${Extra_LIBRARIES})
target_link_libraries(bclient ${Extra_LIBRARIES})

install(TARGETS bserver RUNTIME DESTINATION bin)
install(TARGETS bclient RUNTIME DESTINATION bin)
