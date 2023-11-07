#include "MIGISyncUtilsVulkan.h"

#include "MIGILogCategory.h"

// Include this to get the windows.h to be minimum.
#include "Windows/MinWindows.h"
#include "vulkan/vulkan.h"
#include "IVulkanDynamicRHI.h"


bool FMIGISyncUtilsVulkan::InstallRHIConfigurations()
{
	// Register a delegate to watch for loaded modules.
	// This delegate should be shot after module loading but before RHI initialization (that is, CreateDynamicRHI()).
	// See interface IDynamicRHI for more.
	RHIExtensionRegistrationDelegateHandle = FModuleManager::Get().OnModulesChanged().AddStatic(
		[](FName ModuleName, EModuleChangeReason ModuleChangeReason)
		{
			UE_LOG(MIGI, Display, TEXT("Insert required Vulkan extensions."));
			if(ModuleName == "VulkanRHI")
			{
				// We want to add 2 extensions: VK_KHR_external_semaphore, VK_KHR_external_memory
				// However, we can't check their validity for current platform as vulkan is not even loaded.
				// Just simply insert them into the ExternalExtensions list.
				TArray<const char * > ExtensionNames = {
					VK_KHR_EXTERNAL_SEMAPHORE_EXTENSION_NAME,
					VK_KHR_EXTERNAL_MEMORY_EXTENSION_NAME
				};
				IVulkanDynamicRHI::AddEnabledDeviceExtensionsAndLayers(ExtensionNames, {});
			}
		}
	);
	// Oops
	return true;
}

bool FMIGISyncUtilsVulkan::TryActivate()
{
	// This function is bound to the PostPreStartScreen.
	// Vulkan RHI (if chosen) should be initialized. Just check if it's valid.
	if(GetDynamicRHI<IVulkanDynamicRHI>() == nullptr)
	{
		return false;
	}
	UE_LOG(MIGI, Display, TEXT("Successfully activated RHI-CUDA synchronization utilities for Vulkan."));
	return true;
}

FMIGISyncUtilsVulkan::~FMIGISyncUtilsVulkan()
{
	UE::Core::Private::GetModuleManagerSingleton()->OnModulesChanged().Remove(
		RHIExtensionRegistrationDelegateHandle
	);
	RHIExtensionRegistrationDelegateHandle.Reset();
}

void FMIGISyncUtilsVulkan::SynchronizeFromCUDA(FRHICommandListImmediate& RHICmdList)
{
	// TODO
}


void FMIGISyncUtilsVulkan::SynchronizeToCUDA(FRHICommandListImmediate& RHICmdList)
{
	// TODO
}