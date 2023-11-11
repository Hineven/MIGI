#include "MIGIModule.h"

#include "MIGIDiffuseIndirect.h"
#include "MIGILogCategory.h"

#include "ID3D12DynamicRHI.h"


#define LOCTEXT_NAMESPACE "MIGIModule"

void FMIGIModule::StartupModule()
{
	// Display a logging message on startup
		UE_LOG(MIGI, Display, TEXT("MIGI: Module Starting"));

	// The loading phase of this plugin is "EarliestPossible", so we can do some RHI configurations here.
	// GDynamicRHI is not initialized now.
	check(GDynamicRHI == nullptr);

	FCoreDelegates::OnPostEngineInit.AddLambda([this]()
	{
		// Check for D3D availability.
		if(GetID3D12DynamicRHI() != nullptr) {
			// Call the actual initialization function.
			InitializeMIGI();
		} else UE_LOG(MIGI, Warning, TEXT("D3D is the only supported RHI at the moment. MIGI will not be active."));
	});
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
}


bool FMIGIModule::isActive () const { return bModuleActive; }

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMIGIModule, MIGIEditorMode)