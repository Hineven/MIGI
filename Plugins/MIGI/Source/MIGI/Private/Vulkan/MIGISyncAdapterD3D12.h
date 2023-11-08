#pragma once

#include "CoreMinimal.h"
#include "..\MIGISyncAdapter.h"

class MIGISyncUtilsD3D12State;

class FMIGISyncUtilsD3D12 : public IMIGISyncAdapter
{
public:
	virtual bool InstallRHIConfigurations() override;
	virtual void SynchronizeFromCUDA(FRHICommandListImmediate& RHICmdList) override;
	virtual void SynchronizeToCUDA(FRHICommandListImmediate& RHICmdList) override;
	virtual ~FMIGISyncUtilsD3D12() override;
private:
	virtual bool TryActivate() override;
	FDelegateHandle RHIExtensionRegistrationDelegateHandle;
	// Prevent multiple extension requests.
	bool bExtensionsRequested {};
	
	TUniquePtr<MIGISyncUtilsD3D12State> State {};
};