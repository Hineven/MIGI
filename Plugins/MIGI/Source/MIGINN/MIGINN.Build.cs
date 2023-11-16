using System.IO;
using System;
using UnrealBuildTool;

public class MIGINN : ModuleRules
{
    public MIGINN(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        Type = ModuleType.External;

        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "include"));
        // This library is compiled from CMake.
        PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "lib/MIGINN-Release.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "lib/tiny-cuda-nn-Release.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "lib/fmt-Release.lib"));
        // Find the CUDA library directory.
        string CUDALibDirectoryBase = Path.Combine(Environment.GetEnvironmentVariable("CUDA_PATH"), "lib");
        string CUDALibDirectory = Path.Combine(CUDALibDirectoryBase, Target.Platform.ToString());
        if (!Directory.Exists(CUDALibDirectory))
        {
            // Try plan 2.
            CUDALibDirectory = Path.Combine(CUDALibDirectoryBase, "x64");
            // Report an error if not found.
            if (!Directory.Exists(CUDALibDirectory)) {
                throw new BuildException("CUDA library directory not found in " + CUDALibDirectory + ". Please install CUDA.");
            }
        }
        // Okay we statically link with the CUDA libraries found on the system.
        PublicAdditionalLibraries.Add(Path.Combine(CUDALibDirectory, "cuda.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(CUDALibDirectory, "cudart_static.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(CUDALibDirectory, "cublas.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(CUDALibDirectory, "cudnn.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(CUDALibDirectory, "curand.lib"));
    }
}