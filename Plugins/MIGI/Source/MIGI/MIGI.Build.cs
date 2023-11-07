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
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
	    // Solution from https://forums.unrealengine.com/t/how-to-include-private-header-files-of-other-modules-into-my-module/325438/2
		string EnginePath = Path.GetFullPath(Target.RelativeEnginePath);
		// Now get the base of UE's modules dir (could also be Developer, Editor, ThirdParty)
		string EngineRuntimeModulesSourceDirectoryPath = EnginePath + "Source/Runtime/";
		string EngineThirdpartyModulesSourceDirectoryPath = EnginePath + "Source/ThirdParty/";
    
		// We need this to tightly couple our module with the renderer
		PrivateIncludePaths.Add(EngineRuntimeModulesSourceDirectoryPath + "Renderer/Private");
		// As well as headers of the vulkan RHI. We need low level access to the vulkan API in order to synchronize with CUDA.
		PrivateIncludePaths.Add(EngineRuntimeModulesSourceDirectoryPath + "VulkanRHI/Private");
		// The generic Vulkan headers are also needed.
		// (Deep invasive programming is required to do better synchronization with CUDA other than spin-locks.)
		PrivateIncludePaths.Add(EngineThirdpartyModulesSourceDirectoryPath + "Vulkan/Include");
		// D3D is not supported by MIGI currently (as the maintainer doesn't have that ability).
		
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
				// Also, RHI abstraction layer. 
				"RHI",
				// Specifically the Vulkan RHI
				"VulkanRHI", "Vulkan"
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