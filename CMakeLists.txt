cmake_minimum_required(VERSION 3.13)

project(tracer)

set(CMAKE_CXX_STANDARD 20)

# set(CMAKE_CXX_COMPILER "/usr/bin/clang++")

include_directories("${PROJECT_SOURCE_DIR}/dependencies")
set(EIGEN_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/dependencies/Eigen") 


add_library(ppmpp STATIC dependencies/ppmpp/ppmpp.cpp)
target_include_directories(ppmpp PUBLIC dependencies/ppmpp)

add_library(tinyobjloader STATIC dependencies/tinyobjloader/tiny_obj_loader.cc)
target_include_directories(tinyobjloader PUBLIC dependencies/tinyobjloader)

add_library(eigen INTERFACE)
target_include_directories(eigen INTERFACE ${EIGEN_INCLUDE_DIR})

add_library(openmp INTERFACE)
target_include_directories(openmp INTERFACE "dependencies/OpenMP/include")


file(GLOB SRC_FILES 
	"${PROJECT_SOURCE_DIR}/src/*.h"
	"${PROJECT_SOURCE_DIR}/src/*.c"
	"${PROJECT_SOURCE_DIR}/src/*.hpp"
	"${PROJECT_SOURCE_DIR}/src/*.cpp"
    )

add_executable(${CMAKE_PROJECT_NAME} ${SRC_FILES})

target_link_libraries(${CMAKE_PROJECT_NAME} ppmpp tinyobjloader eigen openmp)

# if you are using neovim with coc-clangd, than you should generate a file using 
# ```
# cmake .. -DCMAKE_EXPORT_COMPILE_COMMANDS=1
# ```
# in your build directery to avoid error message by coc-clangd.
