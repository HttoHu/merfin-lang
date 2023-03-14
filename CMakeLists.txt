cmake_minimum_required(VERSION 3.13.4)
project(merdog5)


set(CMAKE_CXX_STANDARD 17)
# /usr/local/Cellar/llvm/15.0.7_1/lib/cmake/llvm
set(LLVM_DIR "" CACHE PATH "path to llvm cmake dir")
find_package(LLVM REQUIRED 14)
message(STATUS "Architecture: ${TARGET_ARCH}" )
llvm_map_components_to_libnames(llvm_all ${LLVM_TARGETS_TO_BUILD} Passes ExecutionEngine MCJIT)

file(GLOB mer_src "./src/*.cpp")

add_executable(${PROJECT_NAME} ${mer_src})

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
    ${CMAKE_CURRENT_BINARY_DIR}/includes
    ${LLVM_INCLUDE_DIRS}
)
target_link_libraries(${PROJECT_NAME} PUBLIC ${llvm_all})