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
		// As well as headers of some RHIs. We need low level access to them in order to synchronize with CUDA.
		PrivateIncludePaths.Add(EngineRuntimeModulesSourceDirectoryPath + "VulkanRHI/Private");
		PrivateIncludePaths.Add(EngineRuntimeModulesSourceDirectoryPath + "D3D12RHI/Private");
		
		// We have to include more dependencies to use the headers above.
		AddEngineThirdPartyPrivateStaticDependencies(Target, "DX12");
		AddEngineThirdPartyPrivateStaticDependencies(Target, "NVAPI");

		if (Target.Platform.IsInGroup(UnrealPlatformGroup.Windows))
		{
			PrivateDependencyModuleNames.Add("HeadMountedDisplay");

			PublicDefinitions.Add("D3D12RHI_PLATFORM_HAS_CUSTOM_INTERFACE=0");

			if (Target.WindowsPlatform.bPixProfilingEnabled &&
			    (Target.Configuration != UnrealTargetConfiguration.Shipping || Target.bAllowProfileGPUInShipping) &&
			    (Target.Configuration != UnrealTargetConfiguration.Test || Target.bAllowProfileGPUInTest))
			{
				PublicDefinitions.Add("PROFILE");
				PublicDependencyModuleNames.Add("WinPixEventRuntime");
			}
		}
		
		// The generic Vulkan headers are also needed.
		PrivateIncludePaths.Add(EngineThirdpartyModulesSourceDirectoryPath + "Vulkan/Include");
		
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
				"RHI",
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
