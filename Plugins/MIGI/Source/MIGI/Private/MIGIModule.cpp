#include "MIGIModule.h"

#include "MIGIDiffuseIndirect.h"
#include "MIGILogCategory.h"
#include "MIGICUDAAdapter.h"

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
	IMIGICUDAAdapter::Clear();
	IMIGICUDAAdapter::InstallForAllRHIs();

	// Try to activate RHI sync utils after RHI initialization.
	FCoreDelegates::OnPostEngineInit.AddLambda(
		[this]()
		{
			if(IMIGICUDAAdapter::GetInstance() == nullptr)
			{
				UE_LOG(MIGI, Warning, TEXT("RHI-CUDA synchronization utilities are not available for current RHI."
							   "MIGI will be unavaliable."));
				bAdapterActive = false;
			} else bAdapterActive = true;
			if(bCUDAActive && bAdapterActive)
			{
				// Call the actual initialization function.
				InitializeMIGI();
			}
		}
	);
	FCoreDelegates::OnPostEngineInit.AddLambda([this]()
	{
		// Check for CUDA availability.
		auto bCUDAModule = FModuleManager::Get().IsModuleLoaded("CUDA");
		if(!bCUDAModule)
		{
			UE_LOG(MIGI, Warning, TEXT("CUDA is not available, MIGI will be unavaliable."));
			bCUDAActive = false;
		} else bCUDAActive = true;
		auto CUDAModule = FModuleManager::GetModuleChecked<FCUDAModule>("CUDA");
		// CUDA real initialization is done on PostEngineInit() phase, we don't know the actual loading order.
		if(CUDAModule.IsAvailable())
		{
			bCUDAActive = true;
		} else
		{
			// If not available, we register a delegate to check for CUDA availability later again.
			CUDAModule.OnPostCUDAInit.AddLambda([this]()
			{
				auto CUDAModule = FModuleManager::GetModuleChecked<FCUDAModule>("CUDA");
				if(CUDAModule.IsAvailable())
				{
					bCUDAActive = true;
				} else
				{
					UE_LOG(MIGI, Warning, TEXT("CUDA is not available, MIGI will be unavaliable."));
					bCUDAActive = false;
				}
			});
		}
	});

	// Call the actual initialization function.
	if(bModuleActive) InitializeMIGI();
	
	// Check for CUDA availability in later stages.
	// oops
}

void FMIGIModule::InitializeMIGI()
{
	if(!bModuleActive)
	{
		UE_LOG(MIGI, Display, TEXT("Initializing..."));
		// Register some delegates
		DiffuseIndirectDelegateHandle = FGlobalIlluminationPluginDelegates::RenderDiffuseIndirectLight().AddStatic(&MIGIRenderDiffuseIndirect);
		bModuleActive = true;
	}
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
	IMIGICUDAAdapter::Clear();

}


bool FMIGIModule::isActive () const { return bModuleActive; }

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMIGIModule, MIGIEditorMode)