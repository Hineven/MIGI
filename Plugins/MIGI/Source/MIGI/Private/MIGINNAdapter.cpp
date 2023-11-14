#include "MIGINNAdapter.h"

#include "MIGILogCategory.h"
#include "Adapters/MIGINNAdapterD3D12.h"

// static TUniquePtr<IMIGICUDAAdapter> SyncUtilsVulkan;
static TUniquePtr<IMIGINNAdapter> AdapterD3D12;
static TUniquePtr<IMIGINNAdapter> AdapterSelected;

FSimpleMulticastDelegate IMIGINNAdapter::OnAdapterActivated;
size_t IMIGINNAdapter::SharedInputBufferSize;
size_t IMIGINNAdapter::SharedOutputBufferSize;

// This function is executed in the PreEarlyStartupScreen phase.
void IMIGINNAdapter::Install(size_t InSharedInputBufferSize, size_t InSharedOutputBufferSize)
{
	SharedInputBufferSize = InSharedInputBufferSize;
	SharedOutputBufferSize = InSharedOutputBufferSize;
	// SyncUtilsVulkan = MakeUnique<FMIGICUDAAdapterVulkan>();
	// SyncUtilsVulkan->InstallRHIConfigurations();
	AdapterD3D12 = MakeUnique<FMIGICUDAAdapterD3D12>();
	AdapterD3D12->InstallRHIConfigurations();

	// Install the TryActivate function.
	FCoreDelegates::OnInit.AddStatic(TryActivate);
}

void IMIGINNAdapter::TryActivate()
{
	// Already Active
	if(AdapterSelected) return ;
	UE_LOG(MIGI, Display, TEXT("Initialiing MIGINN library."));
	if(!AdapterSelected && AdapterD3D12->CanActivate()) AdapterSelected = std::move(AdapterD3D12);
	// if(!AdapterSelected && AdapterVulkan->CanActivate()) AdapterSelected = std::move(AdapterVulkan);

	// Not supported platform.
	if(!AdapterSelected)
	{
		UE_LOG(MIGI, Warning, TEXT("Not supported platform, MIGI wont be active."));
		return ;
	}

	// Okay activate the selected adapter.
	AdapterSelected->Activate();
	IMIGINNAdapter::OnAdapterActivated.Broadcast();
}


IMIGINNAdapter* IMIGINNAdapter::GetInstance ()
{
	return AdapterSelected.Get();
}

void IMIGINNAdapter::Clear()
{
	// Destructors will take care of everything.
	// SyncUtilsVulkan.Reset();
	AdapterD3D12.Reset();
	AdapterSelected.Reset();
	// Do nothing.
}