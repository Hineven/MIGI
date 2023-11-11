#include "MIGICUDAAdapter.h"

#include "CudaModule.h"
#include "MIGILogCategory.h"
#include "Adapter/MIGICUDAAdapterD3D12.h"

// static TUniquePtr<IMIGICUDAAdapter> SyncUtilsVulkan;
static TUniquePtr<IMIGICUDAAdapter> SyncUtilsD3D12;
static TUniquePtr<IMIGICUDAAdapter> SyncUtilsSelected;

FSimpleMulticastDelegate IMIGICUDAAdapter::OnAdapterActivated;

// This function is executed in the PreEarlyStartupScreen phase.
void IMIGICUDAAdapter::Install()
{
	// SyncUtilsVulkan = MakeUnique<FMIGICUDAAdapterVulkan>();
	// SyncUtilsVulkan->InstallRHIConfigurations();
	SyncUtilsD3D12 = MakeUnique<FMIGICUDAAdapterD3D12>();
	SyncUtilsD3D12->InstallRHIConfigurations();

	// Install the TryActivate function.
	FCoreDelegates::OnInit.AddStatic(TryActivate);
}

void IMIGICUDAAdapter::TryActivatePostCUDAInit ()
{
	// InitCuda() is executed.
	
	// Just try to activate the correct one.
	// if(SyncUtilsVulkan->TryActivate())
	// {
	// 	SyncUtilsSelected = std::move(SyncUtilsVulkan);
	// }
	if(SyncUtilsD3D12->CanActivate())
	{
		SyncUtilsSelected = std::move(SyncUtilsD3D12);
	}

	if(SyncUtilsSelected)
	{
		SyncUtilsSelected->Activate();
		IMIGICUDAAdapter::OnAdapterActivated.Broadcast();
	} else {
		UE_LOG(MIGI, Warning, TEXT("Not supported RHI. MIGI wont be activated."));
	}
}

void IMIGICUDAAdapter::TryActivate()
{
	// Already Active
	if(SyncUtilsSelected) return ;

	if(!FModuleManager::Get().IsModuleLoaded("CUDA"))
	{
		UE_LOG(MIGI, Warning, TEXT("CUDA module is not loaded by default. Request loading..."));
		FModuleManager::Get().LoadModuleChecked("CUDA");
	}
	auto & CUDA = FModuleManager::GetModuleChecked<FCUDAModule>("CUDA");

	
	UE_LOG(MIGI, Warning, TEXT("Registering a deleagte with OnPostCUDAInit"));
	// This function is executed before PostInit(), so the CUDA delegate have always not been broadcast.
	CUDA.OnPostCUDAInit.AddLambda([](){IMIGICUDAAdapter::TryActivatePostCUDAInit();});
}


IMIGICUDAAdapter* IMIGICUDAAdapter::GetInstance ()
{
	return SyncUtilsSelected.Get();
}

void IMIGICUDAAdapter::Clear()
{
	// Destructors will take care of everything.
	// SyncUtilsVulkan.Reset();
	SyncUtilsD3D12.Reset();
	SyncUtilsSelected.Reset();
	// Do nothing.
}