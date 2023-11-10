#include "MIGICUDAAdapter.h"

#include "Vulkan\MIGICUDAAdapterVulkan.h"

static TUniquePtr<IMIGICUDAAdapter> SyncUtilsVulkan;
static TUniquePtr<IMIGICUDAAdapter> SyncUtilsSelected;

void IMIGICUDAAdapter::InstallForAllRHIs()
{
	// We only support Vulkan currently.
	SyncUtilsVulkan = MakeUnique<FMIGICUDAAdapterVulkan>();
	SyncUtilsVulkan->InstallRHIConfigurations();
}

IMIGICUDAAdapter* IMIGICUDAAdapter::GetInstance ()
{
	if(SyncUtilsSelected.IsValid())
		return SyncUtilsSelected.Get();
	// Just try to activate the correct one.
	if(SyncUtilsVulkan->TryActivate())
	{
		SyncUtilsSelected = std::move(SyncUtilsVulkan);
	}
	return SyncUtilsSelected.Get();
}

void IMIGICUDAAdapter::Clear()
{
	// Destructors will take care of everything.
	SyncUtilsVulkan.Reset();
	SyncUtilsSelected.Reset();
	// Do nothing.
}