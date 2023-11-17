cmake_minimum_required(VERSION 3.23)

project(MIGINN)

enable_language(CXX)
enable_language(CUDA)

# Fmtlib (tinycudann supp) can't compile with MSVC C++20 (it has stupid bugs), we'll set the standard to C++17.
set(CMAKE_CXX_STANDARD 17)

# Default directory for .pdb files output.
set(CMAKE_PDB_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

# The cmake test environment malfunctions with CLion, we'll manually set the desired GPU architecture to Ampere.
set(TCNN_CUDA_ARCHITECTURES "89")
set(CMAKE_CUDA_ARCHITECTURES "89")
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

target_link_libraries(MIGINN PUBLIC tiny-cuda-nn ${CUDA_LIBRARIIES} ${CUDA_CPP_LIBRARIES})
target_include_directories(MIGINN PUBLIC ext/tiny-cuda-nn/include)

# Copy the built library file to the output directory.
add_custom_command(TARGET MIGINN POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        $<TARGET_FILE:MIGINN>
        ${CMAKE_SOURCE_DIR}/lib/MIGINN.lib
)
# Copy all dependent libraries to the output directory.
add_custom_command(TARGET MIGINN POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_BINARY_DIR}/ext/tiny-cuda-nn/tiny-cuda-nn.lib
        ${CMAKE_SOURCE_DIR}/lib/tiny-cuda-nn.lib
)
add_custom_command(TARGET MIGINN POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_BINARY_DIR}/ext/tiny-cuda-nn/dependencies/fmt/fmt.lib
        ${CMAKE_SOURCE_DIR}/lib/fmt.lib
)

# Generate .pdb files, functional only for MSVC
# https://stackoverflow.com/questions/28178978/how-to-generate-pdb-files-for-release-build-with-cmake-flags
if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    # TODO This flag works for link.exe, but not for cl.ext
#    target_compile_options(MIGINN PRIVATE "-Xcompiler \"/Zi\"")
#
#    # Tell linker to include symbol data
    set_target_properties(MIGINN PROPERTIES
            LINK_FLAGS "/INCREMENTAL:NO /DEBUG /OPT:REF /OPT:ICF"
    )

    # Set file name & location
    set_target_properties(MIGINN PROPERTIES
            COMPILE_PDB_NAME MIGINN
            COMPILE_PDB_OUTPUT_DIR ${CMAKE_SOURCE_DIR}/lib
    )
else()
    message(WARNING "PDB files will not be generated for this build type.")
endif()

# Add the test executable
add_executable(
        MIGINN_TEST
        src/main.cu
        # Okay we use stbi to load image.
        ext/tiny-cuda-nn/dependencies/stbi/stbi_wrapper.cpp
)
target_link_libraries(MIGINN_TEST PUBLIC tiny-cuda-nn ${CUDA_LIBRARIIES} ${CUDA_CPP_LIBRARIES})
target_include_directories(MIGINN_TEST PUBLIC ext/tiny-cuda-nn/include)