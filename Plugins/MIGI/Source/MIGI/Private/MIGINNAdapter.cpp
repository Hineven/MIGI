#include "MIGINNAdapter.h"

#include "CudaModule.h"
#include "MIGILogCategory.h"
#include "MIGINN.h"
#include "Adapter\MIGINNAdapterD3D12.h"

// static TUniquePtr<IMIGICUDAAdapter> SyncUtilsVulkan;
static TUniquePtr<IMIGINNAdapter> AdapterD3D12;
static TUniquePtr<IMIGINNAdapter> AdapterSelected;

FSimpleMulticastDelegate IMIGINNAdapter::OnAdapterActivated;
size_t IMIGINNAdapter::SharedBufferSize;

// This function is executed in the PreEarlyStartupScreen phase.
void IMIGINNAdapter::Install(size_t InSharedBufferSize)
{
	SharedBufferSize = InSharedBufferSize;
	// SyncUtilsVulkan = MakeUnique<FMIGICUDAAdapterVulkan>();
	// SyncUtilsVulkan->InstallRHIConfigurations();
	AdapterD3D12 = MakeUnique<FMIGICUDAAdapterD3D12>();
	AdapterD3D12->InstallRHIConfigurations();

	// Install the TryActivate function.
	FCoreDelegates::OnInit.AddStatic(TryActivate);
}

void IMIGINNAdapter::TryActivatePostCUDAInit ()
{
	// InitCuda() is executed.
	
	// Just try to activate the correct one.
	// if(SyncUtilsVulkan->TryActivate())
	// {
	// 	SyncUtilsSelected = std::move(SyncUtilsVulkan);
	// }
	if(AdapterD3D12->CanActivate())
	{
		AdapterSelected = std::move(AdapterD3D12);
	}

	if(AdapterSelected)
	{
		AdapterSelected->Activate();
		IMIGINNAdapter::OnAdapterActivated.Broadcast();
	} else {
		UE_LOG(MIGI, Warning, TEXT("Not supported RHI. MIGI wont be activated."));
	}
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
	AdapterSelected->Activate();
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