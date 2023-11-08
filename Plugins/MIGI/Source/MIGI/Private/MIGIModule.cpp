#include "MIGIModule.h"

#include "MIGIDiffuseIndirect.h"
#include "MIGILogCategory.h"
#include "MIGISyncAdapter.h"

#include "CudaModule.h"


#define LOCTEXT_NAMESPACE "MIGIModule"

void FMIGIModule::StartupModule()
{
	// Display a logging message on startup
		UE_LOG(MIGI, Display, TEXT("MIGI: Module Starting"));

	// The loading phase of this plugin is "EarliestPossible", so we can do some RHI configurations here.
	// GDynamicRHI is not initialized now.
	check(GDynamicRHI == nullptr);

	// Initialize and check for RHI-CUDA synchronization support.
	IMIGISyncAdapter::Clear();
	IMIGISyncAdapter::InstallForAllRHIs();

	// Check for CUDA availability in later stages.
	// Also, try to activate RHI sync utils after RHI initialization.
	FCoreDelegates::OnPostEngineInit.AddLambda(
		[this]()
		{
			if(IMIGISyncAdapter::GetInstance() == nullptr)
			{
				UE_LOG(MIGI, Warning, TEXT("RHI-CUDA synchronization utilities are not available for current RHI."
							   "MIGI will be unavaliable."));
				bModuleActive = false;
			} else bModuleActive = true;
			if(bModuleActive)
			{
				// Check for CUDA availability.
				auto bCUDAModule = FModuleManager::Get().IsModuleLoaded("CUDA");
				if(!bCUDAModule
					|| !FModuleManager::GetModuleChecked<FCUDAModule>("CUDA").IsAvailable())
				{
					UE_LOG(MIGI, Warning, TEXT("CUDA is not available, MIGI will be unavaliable."));
					bModuleActive = false;
				} else bModuleActive = true;
				
				// Call the actual initialization function.
				if(bModuleActive) InitializeMIGI();
			}
		}
	);
	// oops
}

void FMIGIModule::InitializeMIGI()
{
	UE_LOG(MIGI, Display, TEXT("Initializing..."));
	// Register some delegates
	DiffuseIndirectDelegateHandle = FGlobalIlluminationPluginDelegates::RenderDiffuseIndirectLight().AddStatic(&MIGIRenderDiffuseIndirect);
}


void FMIGIModule::ShutdownModule()
{
	// Display a logging message on shutdown
	UE_LOG(MIGI, Log, TEXT("Shutting down."));

	// Set module state to inactive.
	bModuleActive = false;
	
	// Clear delegate bindings.
	DiffuseIndirectDelegateHandle.Reset();

	// Clear adapters
	IMIGISyncAdapter::Clear();

}


bool FMIGIModule::isActive () const { return bModuleActive; }

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMIGIModule, MIGIEditorMode)