// Copyright Epic Games, Inc. All Rights Reserved.

#include "MIGIModule.h"

// We need to do direct interaction with vulkan.
#include "vulkan/vulkan.h"

#include "MIGIDiffuseIndirect.h"
#include "MIGILogCategory.h"
#include "MIGISyncUtils.h"
#include "Runtime/VulkanRHI/Public/IVulkanDynamicRHI.h"



#define LOCTEXT_NAMESPACE "MIGIModule"

void FMIGIModule::StartupModule()
{
	// Display a logging message on startup
	UE_LOG(MIGI, Log, TEXT("Module Started"));

	// The loading phase of this plugin is "EarliestPossible", so we can do some RHI configurations here.
	// GDynamicRHI is not initialized now.
	check(GDynamicRHI == nullptr);

	// Initialize and check for RHI-CUDA synchronization support.
	IMIGISyncUtils::Clear();
	IMIGISyncUtils::InstallForAllRHIs();
	
	// Try to activate RHI sync utils after RHI initialization.
	FCoreDelegates::OnPostEngineInit.AddLambda(
		[this]()
		{
			if(IMIGISyncUtils::GetInstance() == nullptr)
			{
				UE_LOG(MIGI, Warning, TEXT("RHI-CUDA synchronization utilities are not available for current RHI."
							   "MIGI will be unavaliable."));
				bModuleFunctional = false;
			} else bModuleFunctional = true;
			if(!bModuleFunctional) return ;
			// Call the actual initialization function.
			InitializeMIGI();
		}
	);

}

void FMIGIModule::InitializeMIGI()
{
	UE_LOG(MIGI, Display, TEXT("Initializing..."));
	// Register some delegates
	FGlobalIlluminationPluginDelegates::RenderDiffuseIndirectLight().AddStatic(&MIGIRenderDiffuseIndirect);
}


void FMIGIModule::ShutdownModule()
{
	// Display a logging message on shutdown
	UE_LOG(MIGI, Log, TEXT("Shutting down."));
	IMIGISyncUtils::Clear();
	bModuleFunctional = false;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMIGIModule, MIGIEditorMode)