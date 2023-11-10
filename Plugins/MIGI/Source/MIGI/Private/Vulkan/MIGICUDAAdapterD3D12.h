#pragma once

#include "CoreMinimal.h"
#include "..\MIGICUDAAdapter.h"

class MIGICUDAAdapterD3D12State;

class FMIGICUDAAdapterD3D12 : public IMIGICUDAAdapter
{
public:
	virtual bool InstallRHIConfigurations() override;
	virtual void SynchronizeFromCUDA(FRHICommandListImmediate& RHICmdList) override;
	virtual void SynchronizeToCUDA(FRHICommandListImmediate& RHICmdList) override;
	virtual ~FMIGICUDAAdapterD3D12() override;
	virtual CUstream GetCUDAStream() const override;
	virtual FRHIBuffer* GetSharedBuffer() const override;
private:
	virtual bool TryActivate() override;
	FDelegateHandle RHIExtensionRegistrationDelegateHandle;
	// Prevent multiple extension requests.
	bool bExtensionsRequested {};
	
	TUniquePtr<MIGICUDAAdapterD3D12State> State {};
};