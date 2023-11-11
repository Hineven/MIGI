// Copyright Epic Games, Inc. All Rights Reserved.

// File ops.
using System.IO;

using UnrealBuildTool;

public class MIGI : ModuleRules
{
	public MIGI(ReadOnlyTargetRules Target) : base(Target) 
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		
	    // Solution from https://forums.unrealengine.com/t/how-to-include-private-header-files-of-other-modules-into-my-module/325438/2
		string EnginePath = Path.GetFullPath(Target.RelativeEnginePath);
		// Now get the base of UE's modules dir (could also be Developer, Editor, ThirdParty)
		string EngineRuntimeModulesSourceDirectoryPath = EnginePath + "Source/Runtime/";
		string EngineThirdpartyModulesSourceDirectoryPath = EnginePath + "Source/ThirdParty/";
    
		// I admit that these are tricky hacks, but I failed to figure out a better way.
		// We need this to tightly couple our module with the renderer
		PrivateIncludePaths.Add(EngineRuntimeModulesSourceDirectoryPath + "Renderer/Private");
		
		// We have to include more dependencies to use the headers above.
		AddEngineThirdPartyPrivateStaticDependencies(Target, "Vulkan");
		AddEngineThirdPartyPrivateStaticDependencies(Target, "DX12");
		AddEngineThirdPartyPrivateStaticDependencies(Target, "NVAPI");
		// This is a public dependency for CUDA module.
		// AddEngineThirdPartyPrivateStaticDependencies(Target, "CUDAHeader");
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core"
				// ... add other public dependencies that you statically link with here ...
			}
			);
		
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"InputCore",
				"RenderCore",
				// We need to statically link with the renderer.
				"Renderer",
				// RHI and its implementations
				"RHI", "RHICore",
				"VulkanRHI", "Vulkan", "D3D12RHI",
				// CUDA
				"CUDA"
			}
		);

		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
		);
	}
}
