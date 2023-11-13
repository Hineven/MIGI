cmake_minimum_required(VERSION 3.23)

project(MIGINN)

enable_language(CXX)
enable_language(CUDA)

# Fmtlib (tinycudann supp) can't compile with MSVC C++20 (it has stupid bugs), we'll set the standard to C++17.
set(CMAKE_CXX_STANDARD 17)

## Still, we need to link the cuda library for MSVC compilation of cpp sources.
#find_package(CUDAToolkit REQUIRED)

# The cmake test environment malfunctions with CLion, we'll manually set the desired GPU architecture to Ampere.
set(TCNN_CUDA_ARCHITECTURES "90")
set(CMAKE_CUDA_ARCHITECTURES "90")
# Add the tiny-cuda-nn library
add_subdirectory(ext/tiny-cuda-nn)

include_directories(include)

add_library(
        MIGINN STATIC
        src/MIGINN.cu
        src/MIGINN_MLP.cu
)

# Link cuda libraries for NVCC compilation
set(CUDA_LIBRARIIES cuda cublas curand cusparse cusolver)
## Link the cuda runtime library for CPP compilation
#set(CUDA_CPP_LIBRARIES CUDA::cudart_static)

target_link_libraries(MIGINN PUBLIC tiny-cuda-nn ${CUDA_LIBRARIIES} ${CUDA_CPP_LIBRARIES})
target_include_directories(MIGINN PUBLIC ext/tiny-cuda-nn/include)

# Copy the built library file to the output directory.
add_custom_command(TARGET MIGINN POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        $<TARGET_FILE:MIGINN>
        ${CMAKE_SOURCE_DIR}/lib/MIGINN.lib
)
