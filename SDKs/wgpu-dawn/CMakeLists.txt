cmake_minimum_required(VERSION 3.26)
project(Dawn)
set(sub_project "Renderer")

file(
   DOWNLOAD
   https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.38.3/CPM.cmake
   ${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake
   EXPECTED_HASH SHA256=cc155ce02e7945e7b8967ddfaff0b050e958a723ef7aad3766d368940cb15494
)
include(${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake)

add_executable(${sub_project} main.cpp)
target_compile_features(${sub_project} PRIVATE cxx_std_17)
set_target_properties(${sub_project} PROPERTIES CXX_EXTENSIONS off)
target_compile_options(${sub_project} PRIVATE -Wall)

set(DAWN_FETCH_DEPENDENCIES ON)
CPMAddPackage(NAME ${PROJECT_NAME} URL "https://dawn.googlesource.com/dawn.git/+archive/refs/heads/main.tar.gz")


target_link_libraries(${sub_project} PRIVATE webgpu_cpp webgpu_dawn webgpu_glfw)