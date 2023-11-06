#pragma once
#ifndef MIGI_SYNC_UTILS_VULKAN_H
#define MIGI_SYNC_UTILS_VULKAN_H
#include "CoreMinimal.h"
#include "MIGISyncUtils.h"

class FMIGISyncUtilsVulkan : public IMIGISyncUtils
{
public:
	virtual bool InstallRHIConfigurations() override;
	virtual void SynchronizeFromCUDA(FRHICommandListImmediate& RHICmdList) override;
	virtual void SynchronizeToCUDA(FRHICommandListImmediate& RHICmdList) override;
	virtual ~FMIGISyncUtilsVulkan() override;
private:
	virtual bool TryActivate() override;
	FDelegateHandle RHIExtensionRegistrationDelegateHandle;
};
#endif // MIGI_SYNC_UTILS_VULKAN_H