### Build & Running Pre-requisites
- CUDA 12.2+
- CMake 3.24+
- MSVC 2019+
- RTX 40 series GPU
- Windows 11
- Unreal Engine 5.3
### Build Steps
1. Clone the repository
2. `cd Source/MIGINN && cmake CMakeLists.txt && cmake --build .`
3. Build this plugin with UE5.
### Running
1. Enable this plugin in Unreal Engine.
2. Set project preference to use "Plugin Provided" global illumination.
