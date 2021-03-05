project(Fibrina)
cmake_minimum_required(VERSION 3.17)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug) # Release for a faster, harder to debug executable
add_executable(main main.cpp src/RandUtils.cpp src/TParticle.cpp src/TLattice.cpp src/TSite.cpp)
target_include_directories(main PUBLIC include)
target_link_libraries(main sfml-graphics sfml-window sfml-system)