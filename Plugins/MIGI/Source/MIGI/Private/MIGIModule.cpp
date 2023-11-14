#include "MIGIModule.h"

#include "Interfaces/IPluginManager.h"

#include "MIGIDiffuseIndirect.h"
#include "MIGILogCategory.h"
#include "MIGINNAdapter.h"

#include "MIGIConstants.h"


#define LOCTEXT_NAMESPACE "MIGIModule"

void FMIGIModule::StartupModule()
{
	// Display a logging message on startup
		UE_LOG(MIGI, Display, TEXT("MIGI: Module Starting"));

	// The loading phase of this plugin is "EarliestPossible", so we can do some RHI configurations here.
	// GDynamicRHI is not initialized now.
	check(GDynamicRHI == nullptr);

	// Add shader mapping.
	auto PluginShaderDir = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("MIGI"))->GetBaseDir(), TEXT("Shaders"));
	AddShaderSourceDirectoryMapping(TEXT("/Plugin/MIGI"), PluginShaderDir);
	
	// Initialize and check for RHI-CUDA synchronization support.
	IMIGINNAdapter::Clear();
	IMIGINNAdapter::Install(C::DefaultSharedBufferSize, C::DefaultSharedBufferSize);
	
	// Callback when the adapter is activated.
	IMIGINNAdapter::OnAdapterActivated.AddLambda([this](){ActivateMIGI();});
}

bool FMIGIModule::ActivateMIGI()
{
	if(bModuleActive) return true;
	if(!IMIGINNAdapter::GetInstance()) return false;
	
	UE_LOG(MIGI, Display, TEXT("Activating..."));
	// Register some delegates
	DiffuseIndirectDelegateHandle = FGlobalIlluminationPluginDelegates::RenderDiffuseIndirectLight().AddStatic(&MIGIRenderDiffuseIndirect);
	bModuleActive = true;
	return true;
}


void FMIGIModule::ShutdownModule()
{
	// Display a logging message on shutdown
	UE_LOG(MIGI, Log, TEXT("Shutting down."));

	if(bModuleActive)
	{
		// Set module state to inactive.
		bModuleActive = false;
		
		// Clear delegate bindings.
		DiffuseIndirectDelegateHandle.Reset();
	}
	// Clear adapters
	IMIGINNAdapter::Clear();

}


bool FMIGIModule::isActive () const { return bModuleActive; }

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMIGIModule, MIGIEditorMode)