#include "MIGISyncAdapter.h"

#include "Vulkan\MIGISyncAdapterVulkan.h"

static TUniquePtr<IMIGISyncAdapter> SyncUtilsVulkan;
static TUniquePtr<IMIGISyncAdapter> SyncUtilsSelected;

void IMIGISyncAdapter::InstallForAllRHIs()
{
	// We only support Vulkan currently.
	SyncUtilsVulkan = MakeUnique<FMIGISyncUtilsVulkan>();
	SyncUtilsVulkan->InstallRHIConfigurations();
}

IMIGISyncAdapter* IMIGISyncAdapter::GetInstance ()
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

void IMIGISyncAdapter::Clear()
{
	// Destructors will take care of everything.
	SyncUtilsVulkan.Reset();
	SyncUtilsSelected.Reset();
	// Do nothing.
}