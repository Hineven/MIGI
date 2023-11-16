using System.IO;
using System;
using System.Diagnostics;
using UnrealBuildTool;

public class MIGINN : ModuleRules
{
    public MIGINN(ReadOnlyTargetRules Target) : base(Target)
    {
        
        // Okay we enable debug symbols for linking this library.
        // https://gamedevtricks.com/post/third-party-libs-3/
        //bTreat = false;      // <- Not necessary but avoids potential issues
        //bPublicSymbolsByDefault = true;  // <- Forced to true on Windows anyways
        //WindowsPlatform.bStripUnreferencedSymbols = false;  // <- Necessary.
        
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        Type = ModuleType.External;

        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "include"));
        // Also import the nlohman json library for parsing the network configuration with ease.
        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "ext/tiny-cuda-nn/dependencies"));
        // This library is compiled from CMake.
        PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "lib/MIGINN-Release.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "lib/tiny-cuda-nn-Release.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "lib/fmt-Release.lib"));
        
        // Find the CUDA library directory.
        string CUDAPath = Environment.GetEnvironmentVariable("CUDA_PATH");
        if (CUDAPath == null)
        {
            throw new BuildException("CUDA_PATH environment variable not found. Please install CUDA.");
        }
        string CUDALibDirectoryBase = Path.Combine(CUDAPath, "lib");
        string CUDALibDirectory = Path.Combine(CUDALibDirectoryBase, Target.Platform.ToString());
        if (!Directory.Exists(CUDALibDirectory))
        {
            // Try Plan B.
            CUDALibDirectory = Path.Combine(CUDALibDirectoryBase, "x64");
            // Report an error if not found.
            if (!Directory.Exists(CUDALibDirectory)) {
                throw new BuildException("CUDA library directory not found in " + CUDALibDirectoryBase + ". Please install CUDA.");
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