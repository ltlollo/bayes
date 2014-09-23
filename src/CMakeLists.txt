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

if("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
    set(LTO_FLAGS "-flto -fno-use-linker-plugin -fwhole-program")
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")
set(CMAKE_CXX_FLAGS_MINSIZEREL "-Os -DNDEBUG -march=native ${LTO_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -march=native ${LTO_FLAGS}")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g ${LTO_FLAGS}")

set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--no-undefined,-02 ${LTO_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${LTO_FLAGS}")
set(CMAKE_MODULE_LINKER_FLAGS "${LTO_FLAGS}")

add_library(libansi ansi.cpp)
add_library(libbayes bayes.cpp)
add_library(libsock socket.cpp)
add_executable(bserver server.cpp)
add_executable(bclient client.cpp)
target_link_libraries(libsock libansi)
target_link_libraries(bserver libbayes)
target_link_libraries(bserver libsock)
target_link_libraries(bclient libsock)

#target_compile_features(libsock PRIVATE cxx_constexpr cxx_auto_type)
#target_compile_features(libansi PRIVATE cxx_constexpr cxx_auto_type)

install(TARGETS bserver RUNTIME DESTINATION bin) 
