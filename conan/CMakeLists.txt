project(PackageTest CXX)
cmake_minimum_required(VERSION 2.8.12)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()


add_executable(the_game ../src/main.cpp ../src/building.cpp ../src/constants.cpp ../src/country.cpp)
target_link_libraries(the_game ${CONAN_LIBS})