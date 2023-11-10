#include "MIGICUDAAdapterVulkan.h"

#include "MIGILogCategory.h"

// Include this to get the windows.h to be minimum.
#include "HardwareInfo.h"
#include "Windows/MinWindows.h"
#include "vulkan/vulkan.h"
#include "IVulkanDynamicRHI.h"


bool FMIGICUDAAdapterVulkan::InstallRHIConfigurations()
{
	// Register a delegate to watch for loaded modules.
	// This delegate should be shot after module loading but before RHI initialization (that is, CreateDynamicRHI()).
	// See interface IDynamicRHI for more.
	RHIExtensionRegistrationDelegateHandle = FModuleManager::Get().OnModulesChanged().AddLambda(
		[this](FName ModuleName, EModuleChangeReason ModuleChangeReason)
		{
			if(ModuleChangeReason == EModuleChangeReason::ModuleLoaded && ModuleName == "VulkanRHI" && bExtensionsRequested)
			{
				bExtensionsRequested = true;
				UE_LOG(MIGI, Display, TEXT("Insert required Vulkan extensions."));
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

bool FMIGICUDAAdapterVulkan::TryActivate()
{
	// This function is bound to the PostPreStartScreen.
	// Vulkan RHI (if chosen) should be initialized. Just check if it's valid.
	if(GDynamicRHI == nullptr)
	{
		return false;
	}
	auto RHIName = GDynamicRHI->GetName();
	// A successful dynamic_cast to IVulkanDynamicRHI does not mean that Vulkan is the actual RHI.
	if(GDynamicRHI->GetInterfaceType() != ERHIInterfaceType::Vulkan)
	{
		return false;
	}
	UE_LOG(MIGI, Display, TEXT("Successfully activated RHI-CUDA synchronization utilities for Vulkan: %s"), RHIName);
	UE_LOG(MIGI, Warning, TEXT("Unreal is not well optimized with Vulkan. Use D3D12 whenever possible."));
	return true;
}

FMIGICUDAAdapterVulkan::~FMIGICUDAAdapterVulkan()
{
	FModuleManager::Get().OnModulesChanged().Remove(
		RHIExtensionRegistrationDelegateHandle
	);
	RHIExtensionRegistrationDelegateHandle.Reset();
}

void FMIGICUDAAdapterVulkan::SynchronizeFromCUDA(FRHICommandListImmediate& RHICmdList)
{
	check(false && "Unimplemented.");
}


void FMIGICUDAAdapterVulkan::SynchronizeToCUDA(FRHICommandListImmediate& RHICmdList)
{
	check(false && "Unimplemented.");
}

CUstream FMIGICUDAAdapterVulkan::GetCUDAStream() const
{
	check(false && "Unimplemented.");
	return nullptr;
}

FRHIBuffer* FMIGICUDAAdapterVulkan::GetSharedBuffer() const
{
	check(false && "Unimplemented.");
	return nullptr;
}
