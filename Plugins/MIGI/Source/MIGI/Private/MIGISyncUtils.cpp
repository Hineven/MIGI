#include "MIGISyncUtils.h"

#include "Vulkan/MIGISyncUtilsVulkan.h"

static TUniquePtr<IMIGISyncUtils> SyncUtilsVulkan;
static TUniquePtr<IMIGISyncUtils> SyncUtilsSelected;

void IMIGISyncUtils::InstallForAllRHIs()
{
	// We only support Vulkan currently.
	SyncUtilsVulkan = MakeUnique<FMIGISyncUtilsVulkan>();
	SyncUtilsVulkan->InstallRHIConfigurations();
}

IMIGISyncUtils* IMIGISyncUtils::GetInstance ()
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

void IMIGISyncUtils::Clear()
{
	SyncUtilsVulkan.Reset();
	SyncUtilsSelected.Reset();
	// Do nothing.
}