add_executable(main main.cpp cokus3.cpp src/RandUtils.cpp src/TParticle.cpp src/TLattice.cpp src/TSite.cpp)
target_include_directories(main PUBLIC include)