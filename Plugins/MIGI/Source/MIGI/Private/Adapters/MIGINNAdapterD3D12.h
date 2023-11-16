#pragma once

#include "CoreMinimal.h"
#include "..\MIGINNAdapter.h"

struct CUDA_DRIVER_API_FUNCTION_LIST;
class MIGICUDAAdapterD3D12State;

class FMIGICUDAAdapterD3D12 : public IMIGINNAdapter
{
public:
	virtual bool InstallRHIConfigurations() override;
	virtual void SynchronizeFromNN(FRHICommandList& RHICmdList) override;
	virtual void SynchronizeToNN(FRHICommandList& RHICmdList) override;
	FMIGICUDAAdapterD3D12 () ;
	virtual ~FMIGICUDAAdapterD3D12() override;
	virtual FRHIBuffer* GetSharedInputBuffer() const override;
	virtual FRHIBuffer * GetSharedOutputBuffer() const override;
protected:
	virtual bool CanActivate () const override;
	virtual void Activate() override;
	void Initialize_RenderThread (FRHICommandListImmediate & RHICmd) ;
	FDelegateHandle RHIExtensionRegistrationDelegateHandle;
	TUniquePtr<MIGICUDAAdapterD3D12State> State;
};