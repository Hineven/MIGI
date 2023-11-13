using System.Diagnostics;
using System.IO;
using UnrealBuildTool;

public class MIGINN : ModuleRules
{
    public MIGINN(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        Type = ModuleType.External;

        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "include"));
        // This library is compiled from CMake.
        PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "MIGINN.lib"));
    }
}