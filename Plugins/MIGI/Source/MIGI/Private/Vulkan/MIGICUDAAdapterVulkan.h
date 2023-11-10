#pragma once
#ifndef MIGI_SYNC_UTILS_VULKAN_H
#define MIGI_SYNC_UTILS_VULKAN_H
#include "CoreMinimal.h"
#include "..\MIGICUDAAdapter.h"

// Vulkan has extremely poor performance on Windows & UE5 due to poor adaption.
// So don't use this if possible.
class FMIGICUDAAdapterVulkan : public IMIGICUDAAdapter
{
public:
	virtual bool InstallRHIConfigurations() override;
	virtual void SynchronizeFromCUDA(FRHICommandListImmediate& RHICmdList) override;
	virtual void SynchronizeToCUDA(FRHICommandListImmediate& RHICmdList) override;
	virtual ~FMIGICUDAAdapterVulkan() override;
	virtual CUstream GetCUDAStream() const override;
	virtual FRHIBuffer* GetSharedBuffer() const override;
private:
	virtual bool TryActivate() override;
	FDelegateHandle RHIExtensionRegistrationDelegateHandle;
	// Prevent multiple extension requests.
	bool bExtensionsRequested {};
};
#endif // MIGI_SYNC_UTILS_VULKAN_H