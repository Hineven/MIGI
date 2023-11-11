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
    
		// These are hacks. I failed to figure out a better way.
		// We need the private renderer headers to tightly couple our module with the renderer
		PrivateIncludePaths.Add(EngineRuntimeModulesSourceDirectoryPath + "Renderer/Private");
		// We need low level access to D3D handles.
		PrivateIncludePaths.Add(EngineRuntimeModulesSourceDirectoryPath + "D3D12RHI/Private");
		
		// We have to include more dependencies to use the headers above.
		AddEngineThirdPartyPrivateStaticDependencies(Target, "DX12");
		// We use DirectML to implement neural networks.
		AddEngineThirdPartyPrivateStaticDependencies(Target, "DirectML");
		
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core"
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
				"RHI", "D3D12RHI"
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
